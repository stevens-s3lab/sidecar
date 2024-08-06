//===------- ShadowCallStack.cpp - SCS -> SideStack pass ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// The SideStack pass instruments function prologs/epilogs to check that
// the return address has not been corrupted during the execution of the
// function. The return address is stored in a 'shadow call stack' maintained
// by an external monitor. Communication is via Intel PTWRITE.
//
//===----------------------------------------------------------------------===//

#include "X86.h"
#include "X86InstrBuilder.h"
#include "X86InstrInfo.h"
#include "X86Subtarget.h"

#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

// SideStack: Use 32bit hashes instead of writing the entire return address
#define SHORTSIDE

using namespace llvm;

namespace llvm {
void initializeShadowCallStackPass(PassRegistry &);
}

namespace {

class ShadowCallStack : public MachineFunctionPass {
public:
  static char ID;

  ShadowCallStack() : MachineFunctionPass(ID) {
    initializeShadowCallStackPass(*PassRegistry::getPassRegistry());
    Sidestack = false;
  }

  bool doInitialization(Module &M) override {
    // S3LAB: Check if we are using sidestack
    Sidestack = M.getModuleFlag("SIDESTACK") != nullptr;
    return true;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &Fn) override;

private:
  // Do not instrument leaf functions with this many or fewer instructions. The
  // shadow call stack instrumented prolog/epilog are slightly race-y reading
  // and checking the saved return address, so it is better to not instrument
  // functions that have fewer instructions than the instrumented prolog/epilog
  // race.
  static const size_t SkipLeafInstructions = 3;
  bool Sidestack;
};

// Opcodes
static const unsigned long PushOpcode = 0x00;
static const unsigned long PushOpcodeShort = 0x00;
static const unsigned long PopOpcode= 0x03;
static const unsigned long PopOpcodeShort = 0x03;

char ShadowCallStack::ID = 0;
} // end anonymous namespace.

 // Helper function to add ModR/M references for [Seg: Reg + Offset] memory
 // accesses
 static inline const MachineInstrBuilder &
 addSegmentedMem(const MachineInstrBuilder &MIB, MCPhysReg Seg, MCPhysReg Reg,
                 int Offset = 0) {
   return MIB.addReg(Reg).addImm(1).addReg(0).addImm(Offset).addReg(Seg);
 }

static void addPrologOriginal(MachineFunction &Fn, const TargetInstrInfo *TII,
                      MachineBasicBlock &MBB, const DebugLoc &DL) {
  const MCPhysReg ReturnReg = X86::R10;
  const MCPhysReg OffsetReg = X86::R11;

  auto MBBI = MBB.begin();
  // mov r10, [rsp]
  addDirectMem(BuildMI(MBB, MBBI, DL, TII->get(X86::MOV64rm)).addDef(ReturnReg),
               X86::RSP);
  // xor r11, r11
  BuildMI(MBB, MBBI, DL, TII->get(X86::XOR64rr))
      .addDef(OffsetReg)
      .addReg(OffsetReg, RegState::Undef)
      .addReg(OffsetReg, RegState::Undef);
  // add QWORD [gs:r11], 8
  addSegmentedMem(BuildMI(MBB, MBBI, DL, TII->get(X86::ADD64mi8)), X86::GS,
                  OffsetReg)
      .addImm(8);
  // mov r11, [gs:r11]
  addSegmentedMem(
      BuildMI(MBB, MBBI, DL, TII->get(X86::MOV64rm)).addDef(OffsetReg), X86::GS,
      OffsetReg);
  // mov [gs:r11], r10
  addSegmentedMem(BuildMI(MBB, MBBI, DL, TII->get(X86::MOV64mr)), X86::GS,
                  OffsetReg)
      .addReg(ReturnReg);
}


static void addEpilogOriginal(const TargetInstrInfo *TII, MachineBasicBlock &MBB,
                      MachineInstr &MI, MachineBasicBlock &TrapBB) {
  const DebugLoc &DL = MI.getDebugLoc();

  // xor r11, r11
  BuildMI(MBB, MI, DL, TII->get(X86::XOR64rr))
      .addDef(X86::R11)
      .addReg(X86::R11, RegState::Undef)
      .addReg(X86::R11, RegState::Undef);
  // mov r10, [gs:r11]
  addSegmentedMem(BuildMI(MBB, MI, DL, TII->get(X86::MOV64rm)).addDef(X86::R10),
                  X86::GS, X86::R11);
  // mov r10, [gs:r10]
  addSegmentedMem(BuildMI(MBB, MI, DL, TII->get(X86::MOV64rm)).addDef(X86::R10),
                  X86::GS, X86::R10);
  // sub QWORD [gs:r11], 8
  // This instruction should not be moved up to avoid a signal race.
  addSegmentedMem(BuildMI(MBB, MI, DL, TII->get(X86::SUB64mi8)),
                  X86::GS, X86::R11)
      .addImm(8);
  // cmp [rsp], r10
  addDirectMem(BuildMI(MBB, MI, DL, TII->get(X86::CMP64mr)), X86::RSP)
      .addReg(X86::R10);
  // jne trap
  BuildMI(MBB, MI, DL, TII->get(X86::JCC_1)).addMBB(&TrapBB).addImm(X86::COND_NE);
  MBB.addSuccessor(&TrapBB);
}

static void addEpilogOnlyR10Original(const TargetInstrInfo *TII, MachineBasicBlock &MBB,
                             MachineInstr &MI, MachineBasicBlock &TrapBB) {
  const DebugLoc &DL = MI.getDebugLoc();

  // xor r10, r10
  BuildMI(MBB, MI, DL, TII->get(X86::XOR64rr))
      .addDef(X86::R10)
      .addReg(X86::R10, RegState::Undef)
      .addReg(X86::R10, RegState::Undef);
  // mov r10, [gs:r10]
  addSegmentedMem(BuildMI(MBB, MI, DL, TII->get(X86::MOV64rm)).addDef(X86::R10),
                  X86::GS, X86::R10);
  // mov r10, [gs:r10]
  addSegmentedMem(BuildMI(MBB, MI, DL, TII->get(X86::MOV64rm)).addDef(X86::R10),
                  X86::GS, X86::R10);
  // sub QWORD [gs:0], 8
  // This instruction should not be moved up to avoid a signal race.
  addSegmentedMem(BuildMI(MBB, MI, DL, TII->get(X86::SUB64mi8)), X86::GS, 0)
      .addImm(8);
  // cmp [rsp], r10
  addDirectMem(BuildMI(MBB, MI, DL, TII->get(X86::CMP64mr)), X86::RSP)
      .addReg(X86::R10);
  // jne trap
  BuildMI(MBB, MI, DL, TII->get(X86::JCC_1)).addMBB(&TrapBB).addImm(X86::COND_NE);
  MBB.addSuccessor(&TrapBB);
}


static void addProlog(MachineFunction &Fn, const TargetInstrInfo *TII,
                      MachineBasicBlock &MBB, const DebugLoc &DL) {
  auto MBBI = MBB.begin();

#ifdef SHORTSIDE
	// DATA = (uint32_t)(data ^ (data >> 17))
  addDirectMem(BuildMI(MBB, MBBI, DL, TII->get(X86::MOV64rm))
			.addDef(X86::R10), X86::RSP);
	BuildMI(MBB, MBBI, DL, TII->get(X86::MOV64rr))
		.addDef(X86::R11)
		.addReg(X86::R10);
	BuildMI(MBB, MBBI, DL, TII->get(X86::SHR64ri))
		.addDef(X86::R11)
		.addReg(X86::R11)
		.addImm(17);
	BuildMI(MBB, MBBI, DL, TII->get(X86::XOR32rr))
		.addDef(X86::R10D)
		.addReg(X86::R10D)
		.addReg(X86::R11D);
  // OPCODE takes top 2 bits
  assert(PushOpcodeShort == 0);
  // Just zero out the top 2 bits for zero
	BuildMI(MBB, MBBI, DL, TII->get(X86::AND32ri))
		.addDef(X86::R10D)
		.addReg(X86::R10D)
		.addImm(0x3FFFFFFF);
  // PTWRITE r10d
	BuildMI(MBB, MBBI, DL, TII->get(X86::PTWRITEr)).addDef(X86::R10D);
#else
  // OPCODE takes top 3 bits
  assert(PushOpcode == 0);
  // Top 3 bits should always be zero since we are in user space
  // PTWRITE [rsp]
  addDirectMem(BuildMI(MBB, MBBI, DL, TII->get(X86::PTWRITE64m)), X86::RSP);
#endif
}

static void addPrologLeaf(MachineFunction &Fn, const TargetInstrInfo *TII,
                          MachineBasicBlock &MBB, const DebugLoc &DL,
                          MCPhysReg FreeRegister) {
  // mov REG, [rsp]
  addDirectMem(BuildMI(MBB, MBB.begin(), DL, TII->get(X86::MOV64rm))
              .addDef(FreeRegister),
              X86::RSP);
}

static void addEpilog(const TargetInstrInfo *TII, MachineBasicBlock &MBB,
                      MachineInstr &MI) {
  const DebugLoc &DL = MI.getDebugLoc();

#ifdef SHORTSIDE
  // DATA = (uint32_t)(data ^ (data >> 17))
  addDirectMem(BuildMI(MBB, MI, DL, TII->get(X86::MOV64rm))
              .addDef(X86::R10),
              X86::RSP);
	BuildMI(MBB, MI, DL, TII->get(X86::MOV64rr))
		.addDef(X86::R11)
		.addReg(X86::R10);
	BuildMI(MBB, MI, DL, TII->get(X86::SHR64ri))
		.addDef(X86::R11)
		.addReg(X86::R11)
		.addImm(17);
	BuildMI(MBB, MI, DL, TII->get(X86::XOR32rr))
		.addDef(X86::R10D)
		.addReg(X86::R10D)
		.addReg(X86::R11D);
  // OPCODE takes top 3 bits
  assert(PopOpcodeShort == 3);
  // ORing the opcode work because it is all 1s
	BuildMI(MBB, MI, DL, TII->get(X86::OR32ri))
		.addDef(X86::R10D)
		.addReg(X86::R10D)
		.addImm(PopOpcodeShort << 30);
  // PTWRITE r10
  BuildMI(MBB, MI, DL, TII->get(X86::PTWRITEr))
		.addDef(X86::R10D);
#else
  // OPCODE takes top 3 bits
  BuildMI(MBB, MI, DL, TII->get(X86::MOV64ri))
              .addDef(X86::R10)
              .addImm(PopOpcode << 62);

  // or r10, [rsp]
  // This works because top 3 bits of memory addresses are 0 in user space
  addDirectMem(BuildMI(MBB, MI, DL, TII->get(X86::OR64rm))
              .addDef(X86::R10)
              .addDef(X86::R10),
              X86::RSP);

  // PTWRITE r10
  BuildMI(MBB, MI, DL, TII->get(X86::PTWRITE64r))
              .addDef(X86::R10);
#endif
}

static void addEpilogLeaf(const TargetInstrInfo *TII, MachineBasicBlock &MBB,
                           MachineInstr &MI, MachineBasicBlock &TrapBB,
                           MCPhysReg FreeRegister) {
  const DebugLoc &DL = MI.getDebugLoc();

  // cmp [rsp], REG
  addDirectMem(BuildMI(MBB, MI, DL, TII->get(X86::CMP64mr)), X86::RSP)
     .addReg(FreeRegister);
  // jne trap
  BuildMI(MBB, MI, DL, TII->get(X86::JCC_1)).addMBB(&TrapBB).addImm(X86::COND_NE);
  //BuildMI(MBB, MI, DL, TII->get(X86::R10)).addMBB(&TrapBB); // XXX: Not sure
  //why R10 is used in this version
  MBB.addSuccessor(&TrapBB);
}


static void addEpilogOnlyR10(const TargetInstrInfo *TII, MachineBasicBlock &MBB,
                             MachineInstr &MI) {
  const DebugLoc &DL = MI.getDebugLoc();

#ifdef SHORTSIDE
  // DATA = (uint32_t)(data ^ (data >> 17))
  addDirectMem(BuildMI(MBB, MI, DL, TII->get(X86::MOV64rm))
              .addDef(X86::R10),
              X86::RSP);
	BuildMI(MBB, MI, DL, TII->get(X86::SHR64ri))
		.addDef(X86::R10)
		.addReg(X86::R10)
		.addImm(17);
  addDirectMem(BuildMI(MBB, MI, DL, TII->get(X86::XOR64rm))
              .addDef(X86::R10)
              .addDef(X86::R10),
              X86::RSP);
  // OPCODE takes the top 2 bits
  assert(PopOpcodeShort == 3);
  // ORing the opcode work because it is all 1s
	BuildMI(MBB, MI, DL, TII->get(X86::OR32ri))
		.addDef(X86::R10D)
		.addReg(X86::R10D)
		.addImm(PopOpcodeShort << 30);
  // PTWRITE r10
  BuildMI(MBB, MI, DL, TII->get(X86::PTWRITEr))
		.addDef(X86::R10D);
#else
  // OPCODE takes the top 3 bits
  BuildMI(MBB, MI, DL, TII->get(X86::MOV64ri))
              .addDef(X86::R10)
              .addImm(PopOpcode << 62);

  // or r10, [rsp]
  // This works because top 3 bits of memory addresses are 0 in user space
  addDirectMem(BuildMI(MBB, MI, DL, TII->get(X86::OR64rm))
              .addDef(X86::R10)
              .addDef(X86::R10),
              X86::RSP);

  // PTWRITE r10
  BuildMI(MBB, MI, DL, TII->get(X86::PTWRITE64r))
              .addDef(X86::R10);
#endif
}

bool ShadowCallStack::runOnMachineFunction(MachineFunction &Fn) {
  if (!Fn.getFunction().hasFnAttribute(Attribute::ShadowCallStack) ||
      Fn.getFunction().hasFnAttribute(Attribute::Naked))
    return false;

  if (Fn.empty() || !Fn.getRegInfo().tracksLiveness())
    return false;

  // FIXME: Skip functions that have r10 or r11 live on entry (r10 can be live
  // on entry for parameters with the nest attribute.)
  if (Fn.front().isLiveIn(X86::R10) || Fn.front().isLiveIn(X86::R11))
    return false;

  // FIXME: Skip functions with conditional and r10 tail calls for now.
  bool HasReturn = false;
  for (auto &MBB : Fn) {
    if (MBB.empty())
      continue;

    const MachineInstr &MI = MBB.instr_back();
#if 0 // Old code
    if (MI.isReturn())
      HasReturn = true;

    if (MI.isReturn() && MI.isCall()) {
      dbgs() << Fn.getName() << " has tail call\n";
      if (MI.findRegisterUseOperand(X86::EFLAGS))
        return false;
      // This should only be possible on Windows 64 (see GR64_TC versus
      // GR64_TCW64.)
      if (MI.findRegisterUseOperand(X86::R10) ||
          MI.hasRegisterImplicitUseOperand(X86::R10))
        return false;
    }
#endif
    if (MI.isReturn()) {
      if (MI.isCall()) { // Tail call
        if (MI.findRegisterUseOperand(X86::EFLAGS))
          return false;
        // This should only be possible on Windows 64 (see GR64_TC versus
        // GR64_TCW64.)
        if (MI.findRegisterUseOperand(X86::R10) ||
            MI.hasRegisterImplicitUseOperand(X86::R10))
          return false;
        // TODO: If the function only exists with tail calls to local function 
        // and the target function has been instrumented we can skip it
        // securely. Whether the function is local can only be possibly determined at
        // link time!
#if 0
        const MachineOperand &TargetOp = MI.getOperand(0);
        if (TargetOp.isGlobal()) {
          const GlobalValue *GV = TargetOp.getGlobal();
          if (GV) {
            StringRef TargetName = GV->getName();
            dbgs() << "In " << Fn.getName() << " tail call to: " << TargetName << " could be potentially optimized\n";
          }
        }
#endif
      }
      HasReturn = true;
    }
  }

  if (!HasReturn)
    return false;

  // For leaf functions:
  // 1. Do not instrument very short functions where it would not improve that
  //    function's security.
  // 2. Detect if there is an unused caller-saved register we can reserve to
  //    hold the return address instead of writing/reading it from the shadow
  //    call stack.
  MCPhysReg LeafFuncRegister = X86::NoRegister;
  if (!Fn.getFrameInfo().adjustsStack()) {
    size_t InstructionCount = 0;
    std::bitset<X86::NUM_TARGET_REGS> UsedRegs;
    for (auto &MBB : Fn) {
      for (auto &LiveIn : MBB.liveins())
        UsedRegs.set(LiveIn.PhysReg);
      for (auto &MI : MBB) {
        if (!MI.isDebugValue() && !MI.isCFIInstruction() && !MI.isLabel())
          InstructionCount++;
        for (auto &Op : MI.operands())
          if (Op.isReg() && Op.isDef())
            UsedRegs.set(Op.getReg());
      }
    }

    if (InstructionCount <= SkipLeafInstructions)
      return false;

    std::bitset<X86::NUM_TARGET_REGS> CalleeSavedRegs;
    const MCPhysReg *CSRegs = Fn.getRegInfo().getCalleeSavedRegs();
    for (size_t i = 0; CSRegs[i]; i++)
      CalleeSavedRegs.set(CSRegs[i]);

    const TargetRegisterInfo *TRI = Fn.getSubtarget().getRegisterInfo();
    for (auto &Reg : X86::GR64_NOSPRegClass.getRegisters()) {
      // FIXME: Optimization opportunity: spill/restore a callee-saved register
      // if a caller-saved register is unavailable.
      if (CalleeSavedRegs.test(Reg))
        continue;

      bool Used = false;
      for (MCSubRegIterator SR(Reg, TRI, true); SR.isValid(); ++SR)
        if ((Used = UsedRegs.test(*SR)))
          break;

      if (!Used) {
        LeafFuncRegister = Reg;
        break;
      }
    }
  }

  const bool LeafFuncOptimization = LeafFuncRegister != X86::NoRegister;
  if (LeafFuncOptimization)
    // Mark the leaf function register live-in for all MBBs except the entry MBB
    for (auto I = ++Fn.begin(), E = Fn.end(); I != E; ++I)
      I->addLiveIn(LeafFuncRegister);

  MachineBasicBlock &MBB = Fn.front();
  const MachineBasicBlock *NonEmpty = MBB.empty() ? MBB.getFallThrough() : &MBB;
  const DebugLoc &DL = NonEmpty->front().getDebugLoc();

  const TargetInstrInfo *TII = Fn.getSubtarget().getInstrInfo();
  if (LeafFuncOptimization)
    addPrologLeaf(Fn, TII, MBB, DL, LeafFuncRegister);
  else {
    if (Sidestack)
      addProlog(Fn, TII, MBB, DL);
    else
      addPrologOriginal(Fn, TII, MBB, DL);
  }

  MachineBasicBlock *Trap = nullptr;
  for (auto &MBB : Fn) {
    if (MBB.empty())
      continue;

    MachineInstr &MI = MBB.instr_back();
    if (MI.isReturn()) {
      if (!Trap && (LeafFuncOptimization || !Sidestack)) {
        Trap = Fn.CreateMachineBasicBlock();
        BuildMI(Trap, MI.getDebugLoc(), TII->get(X86::TRAP));
        Fn.push_back(Trap);
      }

      if (LeafFuncOptimization)
        addEpilogLeaf(TII, MBB, MI, *Trap, LeafFuncRegister);
      else if (MI.findRegisterUseOperand(X86::R11)) {
        if (Sidestack)
          addEpilogOnlyR10(TII, MBB, MI);
        else
          addEpilogOnlyR10Original(TII, MBB, MI, *Trap);
      } else {
        if (Sidestack)
          addEpilog(TII, MBB, MI);
        else
          addEpilogOriginal(TII, MBB, MI, *Trap);
      }
    }
  }

  // TODO: Can I mark changed functions with the statement below?
  // Fn.getFunction().addFnAttr("SideSTACKed");

  return true;
}

INITIALIZE_PASS(ShadowCallStack, "shadow-call-stack", "Shadow Call Stack",
                false, false)

FunctionPass *llvm::createShadowCallStackPass() {
  return new ShadowCallStack();
}

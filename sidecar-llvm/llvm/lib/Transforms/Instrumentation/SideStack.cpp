//===- SideStack.cpp - A decoupled shadow-stack ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Instrumentation/SideStack.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/Statistic.h"

using namespace llvm;

namespace llvm {
struct SideStackPass : public FunctionPass {
	static char ID;
	SideStackPass() : FunctionPass(ID) {}

	bool runOnFunction(Function &F) override {
        if (F.getName() == "main"){
            FunctionCallee SideStackInitFunction =
            declareSanitizerInitFunction(*F.getParent(), "__sidestack_init", {});
            IRBuilder<> IRB(&F.front(), F.front().begin());
            IRB.CreateCall(SideStackInitFunction, {});
            InlineAsm *PTW = InlineAsm::get(FunctionType::get(
                IRB.getVoidTy(), {IRB.getInt64Ty()}, false), 
                StringRef("ptwrite $0"), StringRef("r"), true);

            //IRB.CreateCall(PTW, {});
            //ConstantInt* ci = llvm::ConstantInt::get(Type::getInt32Ty());
            //Value* Args1[] = {ci};
            //CallInst* callInst = CallInst::Create(Intrinsic::getDeclaration(F.getParent(), Intrinsic::returnaddress),
            //    &Args1[0], array_endof(Args1), "Call Return Address", InsPt);
            //*/return true;
        }
        return false;
	}
};

char SideStackPass::ID = 0;
static RegisterPass<SideStackPass> X("sidestack-pass", "SideStack Pass");

FunctionPass *createSideStackPass() { return new SideStackPass(); }
}  // end anonymous namespace


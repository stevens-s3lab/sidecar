# SideCar LLVM Modifications for SideCFI, SideStack, and SideASan

This document summarizes the modifications made to LLVM 12.0.1 to support the SideCar policies: SideCFI, SideStack, and SideASan.

---

## Useful Links for Developers

- [LLVM Sanitizer Tutorial and Documentation](https://github.com/trailofbits/llvm-sanitizer-tutorial)

---

# SideASan Modifications

SideASan (SideCar Address Sanitizer) offloads memory error detection to hardware monitors instead of relying on software-maintained shadow memory. This modification involves changes to both LLVM and the ASan runtime.

## Changed Files

### `compiler-rt/lib/asan/asan_mapping.h`

- Updated the ASan memory mapping to integrate with the hardware monitor for decoupled memory error detection.

### `compiler-rt/lib/asan/asan_poisoning.cpp` and `asan_poisoning.h`

- Adjusted poisoning logic to interact with the hardware monitor instead of updating shadow memory directly.
- Functions like `PoisonShadow`, `FastPoisonShadow`, and others now communicate with the hardware monitor.

### `compiler-rt/lib/asan/asan_report.cpp`

- Updated error reporting to work with decoupled ASan, allowing the hardware monitor to handle or forward error reports.

### `compiler-rt/lib/asan/asan_rtl.cpp`

- Modified runtime initialization to support hardware-based memory error detection.

### `compiler-rt/lib/asan/asan_shadow_setup.cpp`

- Prevented shadow memory allocation as the hardware monitor replaces the need for software-maintained shadow memory.

### `compiler-rt/lib/asan/asan_stats.cpp` and `asan_stats.h`

- Added support for collecting performance statistics relevant to decoupled ASan for analysis.

### `compiler-rt/lib/asan/asan_thread.cpp`

- Adjusted thread management to be compatible with the decoupled ASan approach.

### `llvm/lib/Transforms/Instrumentation/AddressSanitizer.cpp`

- Modified the AddressSanitizer LLVM pass to send hardware messages for memory access checks instead of performing inline checks.

### `compiler-rt/lib/sanitizer_common/sanitizer_posix_libcdep.cpp`

- Updated memory mapping functions to prevent shadow memory allocation, aligning with decoupled ASan.

## New Files

### `compiler-rt/lib/asan/decouple_device.cpp` and `decouple_device.h`

- Implements the interface for communicating with the hardware monitor, handling memory access messages and poisoning information.

## Notes on Address Sanitizer Modifications

- Poisoning/unpoisoning APIs: Functions responsible for managing shadow memory were adjusted to interact with the hardware monitor.
- Memory access checks: Memory access verification is now handled by sending information to the hardware monitor, which performs the checks.
- Shadow memory allocation: Initialization functions were modified to prevent allocating shadow memory.

## Testing Adjustments

- Test files (`asan_internal_interface_test.cpp` and `asan_noinst_test.cpp`) were updated to ensure compatibility with decoupled ASan, verifying that ASan functionalities remain correct in the new decoupled setup.

---

# SideCFI Modifications

SideCFI (SideCar Control Flow Integrity) performs CFI checks using a hardware monitor, enhancing security by offloading control flow verification to a separate entity.

## Changed Files

### `llvm/lib/Transforms/IPO/LowerTypeTests.cpp`

- Adjusted to generate hardware messages for CFI checks instead of performing inline checks or creating jump tables.

### `llvm/lib/Transforms/IPO/CrossDSOCFI.cpp`

- Modified to ensure compatibility with SideCFI by preventing redundant CFI checks when using the hardware monitor.

## Clang Modifications

### Modified Files

- `clang/lib/CodeGen/CGExpr.cpp`
- `clang/lib/CodeGen/CodeGenModule.cpp`
- `clang/lib/CodeGen/CodeGenFunction.cpp`
- `clang/lib/CodeGen/CGDeclCXX.cpp`
- `clang/lib/CodeGen/CGClass.cpp`
- `clang/lib/CodeGen/BackendUtil.cpp`

These changes allow Clang to:

- Generate metadata and function attributes needed for SideCFI.
- Disable the slow path, as CFI checks are performed by the hardware monitor.

## New Runtime

### `compiler-rt/lib/dcfi/`

- A new runtime created to interface with the hardware monitor, handling initialization and communication for SideCFI.

## Notes on SideCFI

- CFI checks are decoupled from the main process and handled by the hardware monitor.
- Integration with LLVM passes ensures that the necessary metadata is generated and communicated correctly for runtime enforcement.

---

# SideStack Modifications

SideStack (SideCar Stack Protection) provides a shadow call stack mechanism using a hardware monitor to protect return addresses, enhancing security against stack-based attacks.

## Changed Files

### `llvm/include/llvm/CodeGen/Passes.h` and `llvm/lib/Passes/PassRegistry.def`

- Declared and registered the new SideStack pass, integrating it into LLVM's code generation passes.

### `llvm/lib/Target/X86/X86.h` and `X86TargetMachine.cpp`

- Integrated the ShadowCallStack pass into the X86 target machine's pass pipeline, ensuring that side stack instrumentation is applied during code generation for X86 targets.

## New Files

### `llvm/lib/Target/X86/ShadowCallStack.cpp`

- Implements the ShadowCallStack pass, instrumenting function prologues and epilogues to manage the side stack for return address protection.

### `llvm/lib/Transforms/Instrumentation/SideStack.cpp` and `SideStack.h`

- Implements the SideStack instrumentation pass, enabling functions to use a separate side stack.

## Runtime

### `compiler-rt/lib/sidestack/`

- A new runtime was created to handle initialization and communication with the hardware monitor for managing the side stack.

## Notes on SideStack

- The ShadowCallStack pass instruments code to maintain a shadow call stack, storing return addresses securely using the hardware monitor.

---

# Summary

## SideASan

- Redirects memory error detection from shadow memory to a hardware monitor.
- Modified poisoning/unpoisoning APIs and memory access checks to interact with the hardware monitor.

## SideCFI

- Offloads CFI checks to the hardware monitor.
- Adjusted Clang and LLVM to generate hardware messages for control flow verification.

## SideStack

- Implements a shadow call stack using a hardware monitor.
- Introduces new LLVM passes and a runtime to manage return address protection.

All modifications aim to leverage hardware capabilities to offload traditionally software-enforced protections, enhancing security and potentially reducing overhead.

---

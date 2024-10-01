# SideCar LLVM Modifications for SideCFI, SideStack, and SideASan

This document describes the modifications made to LLVM 12.0.1 to support the SideCar policies: SideCFI, SideStack, and SideASan.

---

## Useful Links for Developers

- [LLVM Sanitizer Tutorial and Documentation](https://github.com/trailofbits/llvm-sanitizer-tutorial)

---

# SideASan Modifications

SideASan (SideCar Address Sanitizer) is a variant of ASan that offloads memory error detection to hardware monitors. Modifications were made to both the LLVM compiler and the compiler-rt runtime to support decoupling ASan's functionality.

## Changed Files

### `compiler-rt/lib/asan/asan_mapping.h`

- Updated the ASan memory mapping to accommodate decoupled shadow memory mechanisms, integrating with hardware monitors for memory error detection.

### `compiler-rt/lib/asan/asan_poisoning.cpp`

- Adjusted poisoning logic to forward poisoning information to the hardware monitor instead of updating shadow memory directly.

### `compiler-rt/lib/asan/asan_poisoning.h`

- Modified to declare new interfaces and adjust existing ones to support decoupled poisoning mechanisms.

### `compiler-rt/lib/asan/asan_report.cpp`

- Updated error reporting to accommodate decoupled ASan, possibly delegating reporting responsibilities to the hardware monitor or external tools.

### `compiler-rt/lib/asan/asan_rtl.cpp`

- Adjusted runtime initialization to support decoupled ASan operations, including initializing hardware tracing and disabling shadow memory allocation.

### `compiler-rt/lib/asan/asan_shadow_setup.cpp`

- Modified shadow memory setup functions to prevent allocation of shadow memory, as the hardware monitor replaces the need for software-maintained shadow memory.

### `compiler-rt/lib/asan/asan_stats.cpp`

- Added performance analysis and statistics collection for decoupled ASan, aiding in evaluating overhead and effectiveness of the decoupled approach.

### `compiler-rt/lib/asan/asan_stats.h`

- Updated to include new data structures and declarations for performance statistics related to decoupled ASan.

### `compiler-rt/lib/asan/asan_thread.cpp`

- Adjusted thread handling to work with decoupled ASan, possibly modifying how per-thread data is managed without relying on shadow memory.

### `compiler-rt/lib/asan/tests/asan_internal_interface_test.cpp`

- Updated tests to reflect changes in internal interfaces due to decoupled ASan modifications, ensuring compatibility and correctness.

### `compiler-rt/lib/asan/tests/asan_noinst_test.cpp`

- Adjusted non-instrumentation tests to be compatible with decoupled ASan changes, ensuring that ASan's functionality remains correct in the new context.

### `llvm/lib/Transforms/Instrumentation/AddressSanitizer.cpp`

- Modified the AddressSanitizer LLVM pass to generate hardware messages for memory access checks instead of traditional shadow memory checks, integrating with the hardware monitor for error detection.

### `compiler-rt/lib/sanitizer_common/sanitizer_posix_libcdep.cpp`

- Adjusted memory mapping functions to prevent allocation of shadow memory regions, aligning with the decoupled ASan approach.

## New Files

### `compiler-rt/lib/asan/decouple_device.cpp`

- Implements the interface to the hardware monitor or decoupled device, handling communication between the ASan runtime and the external mechanism.

### `compiler-rt/lib/asan/decouple_device.h`

- Header file for `decouple_device.cpp`, declaring functions and data structures for interacting with the decoupled hardware.

---

## Notes on Address Sanitizer Modifications

We have modified several ASan APIs to forward poisoning and checking operations to the hardware monitor. Shadow memory allocation has been adjusted to prevent mapping shadow memory, as the hardware monitor handles memory error detection.

### Intercepted APIs and Modifications

- **Poisoning/Unpoisoning Functions**

  - Adjusted functions responsible for poisoning and unpoisoning memory regions to send messages to the hardware monitor instead of updating shadow memory.

- **Memory Access Checks**

  - Modified memory access checking functions to generate hardware messages for loads and stores, allowing the hardware monitor to detect out-of-bounds accesses.

- **Shadow Memory Allocation**

  - Updated shadow memory initialization functions to prevent allocation of shadow memory regions, since they are not needed with the hardware monitor.

### Shadow Memory Initialization

- `InitializeShadowMemory()`

  - Modified to prevent allocation of shadow memory, aligning with the decoupled ASan design.

---

# SideCFI Modifications

SideCFI (SideCar Control Flow Integrity) offloads CFI checks to a hardware monitor, enhancing security by leveraging hardware capabilities.

## Changed Files

### `llvm/lib/Transforms/IPO/LowerTypeTests.cpp`

- Modified to adjust the lowering of type tests to support decoupled CFI checks. Instead of emitting inline checks or jump tables, the pass now generates hardware messages to be handled by the external monitor.

### `llvm/lib/Transforms/IPO/CrossDSOCFI.cpp`

- Adjusted to ensure compatibility with decoupled CFI mechanisms. Modifications prevent the emission of unnecessary CFI checks when using SideCFI.

## New Files

### `compiler-rt/lib/dcfi/`

- A new runtime library created to interface with the hardware monitor for SideCFI, including initialization code and any necessary support functions.

---

## Clang Modifications

To support SideCFI, several modifications were made in Clang to handle the generation of the necessary metadata and function attributes.

### Modified Files

- `clang/lib/CodeGen/CGExpr.cpp`
- `clang/lib/CodeGen/CodeGenModule.cpp`
- `clang/lib/CodeGen/CodeGenFunction.cpp`
- `clang/lib/CodeGen/CGDeclCXX.cpp`
- `clang/lib/CodeGen/CGClass.cpp`
- `clang/lib/CodeGen/BackendUtil.cpp`

These modifications:

- Generate IDs associated with call sites and functions.
- Disable the slow path for CFI checks, as the hardware monitor handles checks.
- Add new sanitizers and attributes to Clang to support SideCFI.

---

# SideStack Modifications

SideStack (SideCar Stack Protection) implements a shadow call stack mechanism to protect return addresses using a hardware monitor.

## Changed Files

### `llvm/lib/IR/Attributes.cpp`

- Added support for a new `sidestack` function attribute to indicate functions that should use the side stack.

### `llvm/include/llvm/CodeGen/Passes.h`

- Declared the creation of the SideStack pass, integrating it into LLVM's code generation passes.

### `llvm/lib/Passes/PassBuilder.cpp`

- Included the SideStack pass in the LLVM pass builder, enabling its integration into the pass pipeline.

### `llvm/lib/Passes/PassRegistry.def`

- Registered the SideStack pass in the LLVM pass registry, making it available for use during code generation.

### `llvm/lib/Target/X86/X86.h`

- Declared the ShadowCallStack pass for the X86 target, which implements the side stack functionality for protecting return addresses.

### `llvm/lib/Target/X86/X86TargetMachine.cpp`

- Integrated the ShadowCallStack pass into the X86 target machine's pass pipeline, ensuring the pass is executed during code generation for X86 targets.

## New Files

### `llvm/lib/Target/X86/ShadowCallStack.cpp`

- Implements the ShadowCallStack pass for the X86 target, which instruments function prologues and epilogues to implement the side stack mechanism, enhancing security by protecting return addresses.

### `llvm/lib/Transforms/Instrumentation/SideStack.cpp`

- Implements the SideStack instrumentation pass, modifying functions to use a separate side stack for return addresses.

### `llvm/lib/Transforms/Instrumentation/SideStack.h`

- Header file for `SideStack.cpp`, declaring necessary classes and functions for the SideStack pass.

---

## Machine Function Pass

The shadow call stack is implemented in a new pass in:

- `llvm/lib/Target/X86/ShadowCallStack.cpp`

This pass instruments function prologues and epilogues to save and restore return addresses from a shadow stack, potentially using hardware instructions like `PTWRITE`.

## Runtime

A new runtime was created under `compiler-rt/lib/sidestack` to handle the initialization and communication with the hardware monitor for SideStack.

---

# Other Notes

## Address Sanitizer Detailed Modifications

Extensive modifications were made to the ASan runtime and LLVM passes to support decoupling.

- **Shadow Memory Allocation**

  - Shadow memory allocation functions have been modified to prevent mapping shadow memory, as the hardware monitor replaces it.

- **APIs**

  - Various ASan APIs have been modified or redirected to interact with the hardware monitor. Functions responsible for poisoning memory regions now send messages to the monitor.

- **Instrumented Checks**

  - The AddressSanitizer LLVM pass (`AddressSanitizer.cpp`) has been modified to generate hardware messages instead of inline shadow memory checks.

- **Runtime Interface**

  - A new interface in `decouple_device.cpp` and `decouple_device.h` handles communication with the hardware monitor.

### Intercepted APIs and Modifications

We have modified several ASan APIs to forward poisoning and checking operations to the hardware monitor.

- **Poisoning/Unpoisoning Functions**

  - Adjusted functions responsible for poisoning and unpoisoning memory regions to send messages to the hardware monitor instead of updating shadow memory.

- **Memory Access Checks**

  - Modified memory access checking functions to generate hardware messages for loads and stores, allowing the hardware monitor to detect out-of-bounds accesses.

- **Shadow Memory Initialization**

  - Updated shadow memory initialization functions to prevent allocation of shadow memory regions, since they are not needed with the hardware monitor.

---

## LLVM-CFI Modifications

### Clang Modifications

Modifications in Clang were made to support SideCFI, including:

- Generating necessary metadata and function attributes.
- Disabling the slow path for CFI checks.
- Adding new sanitizers to Clang.

### LLVM Modifications

- Adjusted `llvm/lib/Transforms/IPO/LowerTypeTests.cpp` to generate hardware messages for CFI checks instead of traditional inline checks or jump tables.

### Runtime

- Created a new runtime under `compiler-rt/lib/dcfi` to handle initialization and communication with the hardware monitor.

---

## LLVM-SCS Modifications

### Machine Function Pass

- Implemented a new pass in `llvm/lib/Target/X86/ShadowCallStack.cpp` to instrument function prologues and epilogues for shadow call stack functionality.

### Runtime

- Created a new runtime under `compiler-rt/lib/sidestack` to handle initialization and communication with the hardware monitor for SideStack.

---

# Incorporation of Developer Notes

We have incorporated the following notes provided during the development process:

---

## Useful Links for Developers

- [LLVM Sanitizer Tutorial and Documentation](https://github.com/trailofbits/llvm-sanitizer-tutorial)

---

## Address Sanitizer Intercepted APIs

Detailed notes were taken on the various functions within ASan that were intercepted or modified to support decoupling. These include:

- **Poisoning and Unpoisoning Functions**

  - Functions like `PoisonShadow`, `FastPoisonShadow`, and `SetShadow` were analyzed and modified to interact with the hardware monitor.

- **Checking Violations**

  - Macros and functions for checking memory accesses (`CHECK_SMALL_REGION`, `__asan_loadN`, `__asan_storeN`, etc.) were modified to forward checks to the hardware monitor.

- **Shadow Memory Allocation**

  - Shadow memory is initialized during ASan's initialization. Functions like `MmapFixed` in `sanitizer_posix_libcdep.cpp` were modified to prevent allocation of shadow memory.

## Other Poisoning APIs

Other functions within compiler-rt that handle poisoning were also modified or considered in the decoupling process.

## LLVM-CFI Modifications

Modifications were made to Clang and LLVM to support SideCFI:

- **Clang Files Modified**

  - `clang/lib/CodeGen/CGExpr.cpp`
  - `clang/lib/CodeGen/CodeGenModule.cpp`
  - `clang/lib/CodeGen/CodeGenFunction.cpp`
  - `clang/lib/CodeGen/CGDeclCXX.cpp`
  - `clang/lib/CodeGen/CGClass.cpp`
  - `clang/lib/CodeGen/BackendUtil.cpp`

- **LLVM Files Modified**

  - `llvm/lib/Transforms/IPO/LowerTypeTests.cpp`

- **Runtime**

  - A new runtime under `compiler-rt/lib/dcfi` was created.

## LLVM-SCS Modifications

- **Machine Function Pass**

  - Implemented in `llvm/lib/Target/X86/ShadowCallStack.cpp`.

- **Runtime**

  - Created under `compiler-rt/lib/sidestack`.

---

Please refer to the source code and comments within for detailed explanations of the modifications and their purposes.

---

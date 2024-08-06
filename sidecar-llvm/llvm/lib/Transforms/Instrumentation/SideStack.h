#ifndef LLVM_TRANSFORMS_INSTRUMENTATION_SIDESTACK_H
#define LLVM_TRANSFORMS_INSTRUMENTATION_SIDESTACK_H

#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"

namespace llvm {
        FunctionPass *createSideStackPass();

} // namespace llvm

#endif // LLVM_TRANSFORMS_INSTRUMENTATION_SIDESTACK_H

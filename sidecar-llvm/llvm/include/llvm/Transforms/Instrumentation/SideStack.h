//===--------- Definition of the SideStack class ----------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the SideStack class.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_TRANSFORMS_INSTRUMENTATION_SIDESTACK_H
#define LLVM_TRANSFORMS_INSTRUMENTATION_SIDESTACK_H

#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"

namespace llvm {
        FunctionPass *createSideStackPass();

} // namespace llvm

#endif // LLVM_TRANSFORMS_INSTRUMENTATION_SIDESTACK_H

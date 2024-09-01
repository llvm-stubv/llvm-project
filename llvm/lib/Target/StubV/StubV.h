//===-- StubV.h - Top-level interface for Stub-V ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// Stub-V back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_STUBV_STUBV_H
#define LLVM_LIB_TARGET_STUBV_STUBV_H

#include "llvm/Target/TargetMachine.h"

namespace llvm {
class FunctionPass;
class PassRegistry;
class StubVTargetMachine;

FunctionPass *createStubVISelDag(StubVTargetMachine &TM,
                                 CodeGenOptLevel OptLevel);

void initializeStubVDAGToDAGISelLegacyPass(PassRegistry &);
} // namespace llvm

#endif // LLVM_LIB_TARGET_STUBV_STUBV_H

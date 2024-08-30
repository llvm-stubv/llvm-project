//===-- StubVTargetMachine.h - Define TargetMachine for Stub-V --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the Stub-V specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_STUBV_STUBVTARGETMACHINE_H
#define LLVM_LIB_TARGET_STUBV_STUBVTARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"

namespace llvm {
class StubVTargetMachine : public LLVMTargetMachine {

public:
  StubVTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     std::optional<Reloc::Model> RM,
                     std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                     bool JIT);
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_STUBV_STUBVTARGETMACHINE_H

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

#include "StubVSubtarget.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class StubVTargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
public:
  StubVTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     std::optional<Reloc::Model> RM,
                     std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                     bool JIT);

  const StubVSubtarget *getSubtargetImpl(const Function &F) const override;
  // DO NOT IMPLEMENT: There is no such thing as a valid default subtarget,
  // subtargets are per-function entities based on the target-specific
  // attributes of each function.
  const StubVSubtarget *getSubtargetImpl() const = delete;

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

protected:
  mutable StringMap<std::unique_ptr<StubVSubtarget>> SubtargetMap;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_STUBV_STUBVTARGETMACHINE_H

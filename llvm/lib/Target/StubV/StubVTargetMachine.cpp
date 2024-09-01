//===-- StubVTargetMachine.cpp - Define TargetMachine for Stub-V ----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implements the info about Stub-V target spec.
//
//===----------------------------------------------------------------------===//

#include "StubVTargetMachine.h"
#include "StubV.h"
#include "StubVTargetObjectFile.h"
#include "TargetInfo/StubVTargetInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeStubVTarget() {
  RegisterTargetMachine<StubVTargetMachine> X(getTheStubV32Target());
}

static StringRef computeDataLayout(const Triple &TT,
                                   const TargetOptions &Options) {
  return "e-m:e-p:32:32-i64:64-n32-S128";
}

static Reloc::Model getEffectiveRelocModel(const Triple &TT,
                                           std::optional<Reloc::Model> RM) {
  return RM.value_or(Reloc::Static);
}

StubVTargetMachine::StubVTargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       std::optional<Reloc::Model> RM,
                                       std::optional<CodeModel::Model> CM,
                                       CodeGenOptLevel OL, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(TT, Options), TT, CPU, FS, Options,
                        getEffectiveRelocModel(TT, RM),
                        getEffectiveCodeModel(CM, CodeModel::Small), OL),
      TLOF(std::make_unique<StubVELFTargetObjectFile>()) {
  initAsmInfo();
}

const StubVSubtarget *
StubVTargetMachine::getSubtargetImpl(const Function &F) const {
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  auto CPU = CPUAttr.isValid() ? CPUAttr.getValueAsString().str() : TargetCPU;
  auto FS = FSAttr.isValid() ? FSAttr.getValueAsString().str() : TargetFS;

  auto &I = SubtargetMap[CPU + FS];
  if (!I) {
    // This needs to be done before we create a new subtarget since any
    // creation will depend on the TM and the code generation flags on the
    // function that reside in TargetOptions.
    resetTargetOptions(F);
    I = std::make_unique<StubVSubtarget>(TargetTriple, CPU, FS, *this);
  }
  return I.get();
}

namespace {

class StubVPassConfig : public TargetPassConfig {
public:
  StubVPassConfig(StubVTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  StubVTargetMachine &getStubVTargetMachine() const {
    return getTM<StubVTargetMachine>();
  }

  bool addInstSelector() override;
};
} // namespace

bool StubVPassConfig::addInstSelector() {
  addPass(createStubVISelDag(getStubVTargetMachine(), getOptLevel()));

  return false;
}

TargetPassConfig *StubVTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new StubVPassConfig(*this, PM);
}

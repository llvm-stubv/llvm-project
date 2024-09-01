//===-- StubVMCTargetDesc.cpp - Stub-V Target Descriptions ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// This file provides Stub-V specific target descriptions.
///
//===----------------------------------------------------------------------===//

#include "StubVMCTargetDesc.h"
#include "StubVInstPrinter.h"
#include "StubVMCAsmInfo.h"
#include "TargetInfo/StubVTargetInfo.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "StubVGenInstrInfo.inc"

#define GET_REGINFO_MC_DESC
#include "StubVGenRegisterInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "StubVGenSubtargetInfo.inc"

using namespace llvm;

static MCAsmInfo *createStubVMCAsmInfo(const MCRegisterInfo &MRI,
                                       const Triple &TT,
                                       const MCTargetOptions &Options) {
  return new StubVMCAsmInfo(TT);
}

static MCRegisterInfo *createStubVMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitStubVMCRegisterInfo(X, StubV::X1);
  return X;
}

static MCSubtargetInfo *createStubVMCSubtargetInfo(const Triple &TT,
                                                   StringRef CPU, StringRef FS) {
  if (CPU.empty())
    CPU = "generic";

  return createStubVMCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, FS);
}

static MCInstrInfo *createStubVMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitStubVMCInstrInfo(X);
  return X;
}

static MCInstPrinter *createStubVMCInstPrinter(const Triple &T,
                                               unsigned SyntaxVariant,
                                               const MCAsmInfo &MAI,
                                               const MCInstrInfo &MII,
                                               const MCRegisterInfo &MRI) {
  return new StubVInstPrinter(MAI, MII, MRI);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeStubVTargetMC() {
  auto &T = getTheStubV32Target();
  TargetRegistry::RegisterMCAsmInfo(T, createStubVMCAsmInfo);
  TargetRegistry::RegisterMCInstrInfo(T, createStubVMCInstrInfo);
  TargetRegistry::RegisterMCRegInfo(T, createStubVMCRegisterInfo);
  TargetRegistry::RegisterMCAsmBackend(T, createStubVAsmBackend);
  TargetRegistry::RegisterMCCodeEmitter(T, createStubVMCCodeEmitter);
  TargetRegistry::RegisterMCInstPrinter(T, createStubVMCInstPrinter);
  TargetRegistry::RegisterMCSubtargetInfo(T, createStubVMCSubtargetInfo);
}

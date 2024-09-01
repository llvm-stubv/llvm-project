//===-- StubVTargetObjectFile.cpp - Stub-V Object Info --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "StubVTargetObjectFile.h"
#include "MCTargetDesc/StubVMCObjectFileInfo.h"
#include "StubVTargetMachine.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCValue.h"

using namespace llvm;

unsigned StubVELFTargetObjectFile::getTextSectionAlignment() const {
  return StubVMCObjectFileInfo::getTextSectionAlignment(
      *getContext().getSubtargetInfo());
}

void StubVELFTargetObjectFile::Initialize(MCContext &Ctx,
                                          const TargetMachine &TM) {
  TargetLoweringObjectFileELF::Initialize(Ctx, TM);
}

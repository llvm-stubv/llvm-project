//===-- StubVSubtarget.cpp - Stub-V Subtarget Information -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the Stub-V specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "StubVSubtarget.h"

using namespace llvm;

#define DEBUG_TYPE "stubv-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "StubVGenSubtargetInfo.inc"

void StubVSubtarget::anchor() {}

StubVSubtarget &StubVSubtarget::initializeSubtargetDependencies(
    const Triple &TT, StringRef CPU, StringRef TuneCPU, StringRef FS) {
  ParseSubtargetFeatures(CPU, TuneCPU, FS);
  return *this;
}

StubVSubtarget::StubVSubtarget(const Triple &TT, StringRef CPU, StringRef FS,
                               const TargetMachine &TM)
    : StubVGenSubtargetInfo(TT, CPU, /*TuneCPU=*/CPU, FS),
      FrameLowering(
          initializeSubtargetDependencies(TT, CPU, /*TuneCPU*/ CPU, FS)),
      InstrInfo(*this), RegInfo(), TLInfo(TM, *this) {}

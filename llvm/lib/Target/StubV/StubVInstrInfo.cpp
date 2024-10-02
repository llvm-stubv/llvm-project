//===-- StubVInstrInfo.cpp - Stub-V Instruction Information -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the Stub-V implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "StubVInstrInfo.h"
#include "MCTargetDesc/StubVMCTargetDesc.h"
#include "StubV.h"
#include "StubVSubtarget.h"
#include "StubVTargetMachine.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#define GET_INSTRINFO_NAMED_OPS
#include "StubVGenInstrInfo.inc"

StubVInstrInfo::StubVInstrInfo(StubVSubtarget &STI)
    : StubVGenInstrInfo(), STI(STI) {}

void StubVInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MBBI,
                                 const DebugLoc &DL, MCRegister DstReg,
                                 MCRegister SrcReg, bool KillSrc) const {
  // We can copy between GPRs using ADDI
  if (StubV::GPRRegClass.contains(DstReg, SrcReg)) {
    BuildMI(MBB, MBBI, DL, get(StubV::ADDI), DstReg)
        .addReg(SrcReg, getKillRegState(KillSrc))
        .addImm(0);
    return;
  }

  // Other than that we don't know what to do yet
  report_fatal_error("Unable to copy phys reg");
}

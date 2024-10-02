//===-- StubVInstrInfo.h - Stub-V Instruction Information -------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_STUBV_STUBVINSTRINFO_H
#define LLVM_LIB_TARGET_STUBV_STUBVINSTRINFO_H

#include "StubV.h"
#include "StubVRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#define GET_INSTRINFO_OPERAND_ENUM
#include "StubVGenInstrInfo.inc"
#include "StubVGenRegisterInfo.inc"

namespace llvm {

class StubVSubtarget;

class StubVInstrInfo : public StubVGenInstrInfo {

public:
  explicit StubVInstrInfo(StubVSubtarget &STI);

  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI,
                   const DebugLoc &DL, MCRegister DstReg, MCRegister SrcReg,
                   bool KillSrc) const override;

protected:
  const StubVSubtarget &STI;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_STUBV_STUBVINSTRINFO_H

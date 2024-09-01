//===-- StubVRegisterInfo.cpp - Stub-V Register Information -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the Stub-V implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "StubVRegisterInfo.h"
#include "MCTargetDesc/StubVMCTargetDesc.h"
#include "StubV.h"
#include "StubVSubtarget.h"

#define GET_REGINFO_TARGET_DESC
#include "StubVGenRegisterInfo.inc"

using namespace llvm;

StubVRegisterInfo::StubVRegisterInfo()
    : StubVGenRegisterInfo(StubV::X1, /*DwarfFlavour*/ 0, /*EHFlavor*/ 0,
                           /*PC*/ 0) {}
const MCPhysReg *
StubVRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_ILP32_LP64_SaveList;
}

bool StubVRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                            int SPAdj, unsigned FIOperandNum,
                                            RegScavenger *RS) const {
  llvm_unreachable("Unimplemented API");
}

BitVector StubVRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  const StubVFrameLowering *TFI = getFrameLowering(MF);
  BitVector Reserved(getNumRegs());

  for (size_t Reg = 0; Reg < getNumRegs(); Reg++) {
    // Mark all the registers defined as constant in TableGen as reserved.
    if (isConstantPhysReg(Reg))
      markSuperRegs(Reserved, Reg);
  }

  // Use markSuperRegs to ensure any register aliases are also reserved
  markSuperRegs(Reserved, StubV::X2); // sp
  markSuperRegs(Reserved, StubV::X3); // gp
  markSuperRegs(Reserved, StubV::X4); // tp

  if (TFI->hasFP(MF))
    markSuperRegs(Reserved, StubV::X8); // fp

  assert(checkAllSuperRegsMarked(Reserved));
  return Reserved;
}

Register StubVRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = getFrameLowering(MF);
  return TFI->hasFP(MF) ? StubV::X8 : StubV::X2;
}

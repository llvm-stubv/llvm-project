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
#include "llvm/CodeGen/MachineFrameInfo.h"

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

void StubVInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                         MachineBasicBlock::iterator I,
                                         Register SrcReg, bool IsKill, int FI,
                                         const TargetRegisterClass *RC,
                                         const TargetRegisterInfo *TRI,
                                         Register VReg) const {
  MachineFunction *MF = MBB.getParent();
  MachineFrameInfo &MFI = MF->getFrameInfo();

  unsigned Opcode;
  if (StubV::GPRRegClass.hasSubClassEq(RC)) {
    assert(TRI->getRegSizeInBits(StubV::GPRRegClass) == 32);
    Opcode = StubV::SW;
  } else
    llvm_unreachable("Can't store this register to stack slot");

  MachineMemOperand *MMO = MF->getMachineMemOperand(
      MachinePointerInfo::getFixedStack(*MF, FI), MachineMemOperand::MOStore,
      MFI.getObjectSize(FI), MFI.getObjectAlign(FI));

  BuildMI(MBB, I, DebugLoc(), get(Opcode))
      .addReg(SrcReg, getKillRegState(IsKill))
      .addFrameIndex(FI)
      .addImm(0)
      .addMemOperand(MMO);
}

void StubVInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator I,
                                          Register DstReg, int FI,
                                          const TargetRegisterClass *RC,
                                          const TargetRegisterInfo *TRI,
                                          Register VReg) const {
  MachineFunction *MF = MBB.getParent();
  MachineFrameInfo &MFI = MF->getFrameInfo();

  unsigned Opcode;
  if (StubV::GPRRegClass.hasSubClassEq(RC)) {
    assert(TRI->getRegSizeInBits(StubV::GPRRegClass) == 32);
    Opcode = StubV::LW;
  } else
    llvm_unreachable("Can't load this register from stack slot");

  MachineMemOperand *MMO = MF->getMachineMemOperand(
      MachinePointerInfo::getFixedStack(*MF, FI), MachineMemOperand::MOLoad,
      MFI.getObjectSize(FI), MFI.getObjectAlign(FI));

  BuildMI(MBB, I, DebugLoc(), get(Opcode), DstReg)
      .addFrameIndex(FI)
      .addImm(0)
      .addMemOperand(MMO);
}

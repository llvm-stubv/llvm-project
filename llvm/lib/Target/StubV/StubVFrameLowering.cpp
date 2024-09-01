//===-- StubVFrameLowering.cpp - Stub-V Frame Information -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the Stub-V implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "StubVFrameLowering.h"
#include "StubVSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"

using namespace llvm;

StubVFrameLowering::StubVFrameLowering(const StubVSubtarget &STI)
    : TargetFrameLowering(StackGrowsDown, /*StackAlign=*/Align(16),
                          /*LocalAreaOffset=*/0,
                          /*TransientStackAlignment=*/Align(16)),
      STI(STI) {}

void StubVFrameLowering::emitPrologue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  llvm_unreachable("Unimplemented API");
}

void StubVFrameLowering::emitEpilogue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  llvm_unreachable("Unimplemented API");
}

// Return true if the specified function should have a dedicated frame
// pointer register.  This is true if frame pointer elimination is
// disabled, if it needs dynamic stack realignment, if the function has
// variable sized allocas, or if the frame address is taken.
bool StubVFrameLowering::hasFP(const MachineFunction &MF) const {
  const TargetRegisterInfo *RegInfo = MF.getSubtarget().getRegisterInfo();

  const MachineFrameInfo &MFI = MF.getFrameInfo();
  return MF.getTarget().Options.DisableFramePointerElim(MF) ||
         RegInfo->hasStackRealignment(MF) || MFI.hasVarSizedObjects() ||
         MFI.isFrameAddressTaken();
}

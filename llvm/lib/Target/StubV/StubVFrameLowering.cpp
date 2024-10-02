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

// Determines the size of the frame and maximum call frame size.
void StubVFrameLowering::determineFrameLayout(MachineFunction &MF) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();

  // Get the number of bytes to allocate from the FrameInfo.
  uint64_t FrameSize = MFI.getStackSize();

  // Get the alignment.
  Align StackAlign = getStackAlign();

  // Make sure the frame is aligned.
  FrameSize = alignTo(FrameSize, StackAlign);

  // Update frame info.
  MFI.setStackSize(FrameSize);
}

void StubVFrameLowering::emitPrologue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();

  // Determine the correct frame layout
  determineFrameLayout(MF);

  uint64_t StackSize = alignTo(MFI.getStackSize(), getStackAlign());

  if (StackSize == 0 && !MFI.adjustsStack())
    return;

  report_fatal_error("No support for non-zero stack usage");
}

void StubVFrameLowering::emitEpilogue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();

  const StubVRegisterInfo *RI = STI.getRegisterInfo();
  assert(!RI->hasStackRealignment(MF) && !MFI.hasVarSizedObjects() &&
         hasReservedCallFrame(MF) && "Stack usage limited");

  uint64_t StackSize = alignTo(MFI.getStackSize(), getStackAlign());

  if (StackSize == 0 && !MFI.adjustsStack())
    return;

  report_fatal_error("No support for non-zero stack usage");
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

// Not preserve stack space within prologue for outgoing variables when the
// function contains variable size objects.
// Let eliminateCallFramePseudoInstr preserve stack space for it.
bool StubVFrameLowering::hasReservedCallFrame(const MachineFunction &MF) const {
  return !MF.getFrameInfo().hasVarSizedObjects();
}

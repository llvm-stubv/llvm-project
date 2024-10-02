//===-- StubVFrameLowering.h - Define frame lowering for Stub-V -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class implements Stub-V specific bits of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_STUBV_STUBVFRAMELOWERING_H
#define LLVM_LIB_TARGET_STUBV_STUBVFRAMELOWERING_H

#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/Support/TypeSize.h"

namespace llvm {
class StubVSubtarget;

class StubVFrameLowering : public TargetFrameLowering {
public:
  explicit StubVFrameLowering(const StubVSubtarget &STI);

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  bool hasFP(const MachineFunction &MF) const override;

protected:
  const StubVSubtarget &STI;

private:
  void determineFrameLayout(MachineFunction &MF) const;
  bool hasReservedCallFrame(const MachineFunction &MF) const override;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_STUBV_STUBVFRAMELOWERING_H

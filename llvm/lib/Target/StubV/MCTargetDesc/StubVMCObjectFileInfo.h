//===-- StubVMCObjectFileInfo.h - Stub-V object file Info ------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the StubVMCObjectFileInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVMCOBJECTFILEINFO_H
#define LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVMCOBJECTFILEINFO_H

#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"

namespace llvm {

class StubVMCObjectFileInfo : public MCObjectFileInfo {
public:
  static unsigned getTextSectionAlignment(const MCSubtargetInfo &STI);
  unsigned getTextSectionAlignment() const override;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVMCOBJECTFILEINFO_H

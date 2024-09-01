//===-- StubVMCTargetDesc.h - Stub-V Target Descriptions --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides Stub-V specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVMCTARGETDESC_H
#define LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVMCTARGETDESC_H

#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/DataTypes.h"
#include <memory>

namespace llvm {
}

// Defines symbolic names for Stub-V registers.
#define GET_REGINFO_ENUM
#include "StubVGenRegisterInfo.inc"

// Defines symbolic names for Stub-V instructions.
#define GET_INSTRINFO_ENUM
#include "StubVGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "StubVGenSubtargetInfo.inc"

#endif // LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVMCTARGETDESC_H

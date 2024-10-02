//===-- StubVBaseInfo.h - Top level definitions for Stub-V MC ---*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone enum definitions for the Stub-V target
// useful for the compiler back-end and the MC libraries.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVBASEINFO_H
#define LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVBASEINFO_H

#include "MCTargetDesc/StubVMCTargetDesc.h"

namespace llvm {

namespace StubVABI {

enum ABI { ABI_ILP32, ABI_Unknown };

} // namespace StubVABI

} // namespace llvm

#endif // LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVBASEINFO_H

//===-- StubVMCAsmInfo.h - Stub-V Asm Info ---------------------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the StubVMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVMCASMINFO_H
#define LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVMCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

class StubVMCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit StubVMCAsmInfo(const Triple &TargetTriple);
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVMCASMINFO_H

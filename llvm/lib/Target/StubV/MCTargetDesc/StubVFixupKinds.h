//===-- StubVFixupKinds.h - Stub-V Specific Fixup Entries -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVFIXUPKINDS_H
#define LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVFIXUPKINDS_H

#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCFixup.h"
#include <utility>

#undef StubV

namespace llvm::StubV {
enum Fixups {
  // Used as a sentinel, must be the last
  fixup_riscv_invalid = FirstTargetFixupKind,
  NumTargetFixupKinds = fixup_riscv_invalid - FirstTargetFixupKind
};
}

#endif // LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVFIXUPKINDS_H

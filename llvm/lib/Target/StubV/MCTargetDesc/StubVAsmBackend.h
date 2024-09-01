//===-- StubVAsmBackend.h - Stub-V Assembler Backend ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVASMBACKEND_H
#define LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVASMBACKEND_H

#include "MCTargetDesc/StubVFixupKinds.h"
#include "MCTargetDesc/StubVMCTargetDesc.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"

namespace llvm {

class StubVAsmBackend : public MCAsmBackend {
  uint8_t OSABI;
public:
  StubVAsmBackend() : MCAsmBackend(llvm::endianness::little) {}
  ~StubVAsmBackend() override = default;

  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const override;

  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override;

  unsigned getNumFixupKinds() const override {
    return StubV::NumTargetFixupKinds;
  }

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_STUBV_MCTARGETDESC_STUBVASMBACKEND_H

//===-- StubVAsmBackend.cpp - Stub-V Assembler Backend --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "StubVAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

std::unique_ptr<MCObjectTargetWriter>
StubVAsmBackend::createObjectTargetWriter() const {
  return createStubVELFObjectWriter(OSABI);
}

static uint64_t adjustFixupValue(const MCFixup &Fixup, uint64_t Value,
                                 MCContext &Ctx) {
  switch (Fixup.getTargetKind()) {
  default:
    llvm_unreachable("Unknown fixup kind!");
  case FK_Data_1:
  case FK_Data_2:
  case FK_Data_4:
  case FK_Data_8:
  case FK_Data_leb128:
    return Value;
  }
}

void StubVAsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                                 const MCValue &Target,
                                 MutableArrayRef<char> Data, uint64_t Value,
                                 bool IsResolved,
                                 const MCSubtargetInfo *STI) const {
  MCFixupKind Kind = Fixup.getKind();
  if (Kind >= FirstLiteralRelocationKind)
    return;
  MCContext &Ctx = Asm.getContext();
  MCFixupKindInfo Info = getFixupKindInfo(Kind);
  if (!Value)
    return; // Doesn't change encoding.
  // Apply any target-specific value adjustments.
  Value = adjustFixupValue(Fixup, Value, Ctx);

  // Shift the value into position.
  Value <<= Info.TargetOffset;

  unsigned Offset = Fixup.getOffset();
  unsigned NumBytes = alignTo(Info.TargetSize + Info.TargetOffset, 8) / 8;

  assert(Offset + NumBytes <= Data.size() && "Invalid fixup offset!");

  // For each byte of the fragment that the fixup touches, mask in the
  // bits from the fixup value.
  for (unsigned i = 0; i != NumBytes; ++i) {
    Data[Offset + i] |= uint8_t((Value >> (i * 8)) & 0xff);
  }
}

bool StubVAsmBackend::writeNopData(raw_ostream &OS, uint64_t Count,
                                   const MCSubtargetInfo *STI) const {
  // We mostly follow binutils' convention here: align to even boundary with a
  // 0-fill padding.  We emit up to 1 2-byte nop, though we use c.nop if RVC is
  // enabled or 0-fill otherwise.  The remainder is now padded with 4-byte nops.

  // Instructions always are at even addresses.  We must be in a data area or
  // be unaligned due to some other reason.
  if (Count % 2) {
    OS.write("\0", 1);
    Count -= 1;
  }

  // The canonical nop on RVC is c.nop.
  if (Count % 4 == 2) {
    OS.write("\0\0", 2);
    Count -= 2;
  }

  // The canonical nop on RISC-V is addi x0, x0, 0.
  for (; Count >= 4; Count -= 4)
    OS.write("\x13\0\0\0", 4);

  return true;
}

MCAsmBackend *llvm::createStubVAsmBackend(const Target &T,
                                          const MCSubtargetInfo &STI,
                                          const MCRegisterInfo &MRI,
                                          const MCTargetOptions &Options) {
  return new StubVAsmBackend();
}

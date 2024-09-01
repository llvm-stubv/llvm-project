//===-- StubVELFObjectWriter.cpp - Stub-V ELF Writer ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/StubVFixupKinds.h"
#include "MCTargetDesc/StubVMCTargetDesc.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
class StubVELFObjectWriter : public MCELFObjectTargetWriter {

public:
  StubVELFObjectWriter(uint8_t OSABI);

  ~StubVELFObjectWriter() override;

  // Return true if the given relocation must be with a symbol rather than
  // section plus offset.
  bool needsRelocateWithSymbol(const MCValue &Val, const MCSymbol &Sym,
                               unsigned Type) const override {
    // TODO: this is very conservative, update once RISC-V psABI requirements
    //       are clarified.
    return true;
  }

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};
}

StubVELFObjectWriter::StubVELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(/*Is64Bit*/ false, OSABI, ELF::EM_RISCV,
                              /*HasRelocationAddend*/ true) {}

StubVELFObjectWriter::~StubVELFObjectWriter() = default;

unsigned StubVELFObjectWriter::getRelocType(MCContext &Ctx,
                                            const MCValue &Target,
                                            const MCFixup &Fixup,
                                            bool IsPCRel) const {
  // Determine the type of the relocation
  unsigned Kind = Fixup.getTargetKind();
  if (Kind >= FirstLiteralRelocationKind)
    return Kind - FirstLiteralRelocationKind;

  if (IsPCRel) {
    switch (Kind) {
    default:
      Ctx.reportError(Fixup.getLoc(), "unsupported relocation type");
      return ELF::R_RISCV_NONE;
    case FK_Data_4:
    case FK_PCRel_4:
      return Target.getAccessVariant() == MCSymbolRefExpr::VK_PLT
                 ? ELF::R_RISCV_PLT32
                 : ELF::R_RISCV_32_PCREL;
    }
  }

  dbgs() << "type: " << Kind << "\n";
  Ctx.reportError(Fixup.getLoc(), "unsupported relocation type");
  return ELF::R_RISCV_NONE;
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createStubVELFObjectWriter(uint8_t OSABI) {
  return std::make_unique<StubVELFObjectWriter>(OSABI);
}

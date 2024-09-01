//===-- StubVInstPrinter.cpp - Convert Stub-V MCInst to asm syntax --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class prints an Stub-V MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "StubVInstPrinter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCRegisterInfo.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

// Include the auto-generated portion of the assembly writer.
#define PRINT_ALIAS_INSTR
#include "StubVGenAsmWriter.inc"

void StubVInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                 StringRef Annot, const MCSubtargetInfo &STI,
                                 raw_ostream &O) {
  const MCInst *NewMI = MI;
  MCInst UncompressedMI;
  if (!printAliasInstr(NewMI, Address, O))
    printInstruction(NewMI, Address, O);
  printAnnotation(O, Annot);
}

void StubVInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                    raw_ostream &O, const char *Modifier) {
  assert((Modifier == nullptr || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &MO = MI->getOperand(OpNo);

  if (MO.isReg()) {
    printRegName(O, MO.getReg());
    return;
  }

  if (MO.isImm()) {
    markup(O, Markup::Immediate) << formatImm(MO.getImm());
    return;
  }

  assert(MO.isExpr() && "Unknown operand kind in printOperand");
  MO.getExpr()->print(O, &MAI);
}

void StubVInstPrinter::printRegName(raw_ostream &O, MCRegister Reg) const {
  markup(O, Markup::Register) << getRegisterName(Reg);
}

// Print architectural register names rather than the ABI names (such as x2
// instead of sp).
// TODO: Make StubVInstPrinter::getRegisterName non-static so that this can a
// member.
static bool ArchRegNames;

const char *StubVInstPrinter::getRegisterName(MCRegister Reg) {
  return getRegisterName(Reg, ArchRegNames ? StubV::NoRegAltName
                                           : StubV::ABIRegAltName);
}

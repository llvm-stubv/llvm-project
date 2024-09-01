//===-- StubVAsmPrinter.cpp - Stub-V LLVM assembly writer -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the Stub-V assembly language.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/StubVInstPrinter.h"
#include "StubV.h"
#include "StubVTargetMachine.h"
#include "TargetInfo/StubVTargetInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {
class StubVAsmPrinter : public AsmPrinter {
public:
  explicit StubVAsmPrinter(TargetMachine &TM,
                           std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {}

  StringRef getPassName() const override { return "Stub-V Assembly Printer"; }

  bool runOnMachineFunction(MachineFunction &MF) override;

  void emitInstruction(const MachineInstr *MI) override;

  bool lowerOperand(const MachineOperand &MO, MCOperand &MCOp) const;

private:
  bool lowerToMCInst(const MachineInstr *MI, MCInst &OutMI);
};
} // namespace

void StubVAsmPrinter::emitInstruction(const MachineInstr *MI) {
  MCInst OutInst;
  if (!lowerToMCInst(MI, OutInst))
    EmitToStreamer(*OutStreamer, OutInst);
}

bool StubVAsmPrinter::lowerToMCInst(const MachineInstr *MI, MCInst &OutMI) {
  OutMI.setOpcode(MI->getOpcode());

  for (const MachineOperand &MO : MI->operands()) {
    MCOperand MCOp;
    if (lowerOperand(MO, MCOp))
      OutMI.addOperand(MCOp);
  }

  return false;
}

bool StubVAsmPrinter::lowerOperand(const MachineOperand &MO,
                                   MCOperand &MCOp) const {
  switch (MO.getType()) {
  default:
    report_fatal_error("lowerOperand: unknown operand type");
  case MachineOperand::MO_Register:
    // Ignore all implicit register operands.
    if (MO.isImplicit())
      return false;
    MCOp = MCOperand::createReg(MO.getReg());
    break;
  case MachineOperand::MO_RegisterMask:
    // Regmasks are like implicit defs.
    return false;
  case MachineOperand::MO_Immediate:
    MCOp = MCOperand::createImm(MO.getImm());
    break;
  }
  return true;
}

bool StubVAsmPrinter::runOnMachineFunction(MachineFunction &MF) {
  SetupMachineFunction(MF);
  emitFunctionBody();
  return false;
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeStubVAsmPrinter() {
  RegisterAsmPrinter<StubVAsmPrinter> X(getTheStubV32Target());
}

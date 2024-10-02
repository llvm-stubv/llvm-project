//===-- StubVInstrInfo.td - Target Description for Stub-V --*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes the Stub-V instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Stub-V specific DAG Nodes.
//===----------------------------------------------------------------------===//

include "StubVInstrFormats.td"

def ADD : Instruction {
  let Namespace = "StubV";
  dag OutOperandList = (outs GPR:$rd);
  dag InOperandList = (ins GPR:$rs1, GPR:$rs2);
  let AsmString = "add\t$rd, $rs1, $rs2";

  field bits<32> Inst;
  // SoftFail is a field the disassembler can use to provide a way for
  // instructions to not match without killing the whole decode process. It is
  // mainly used for ARM, but Tablegen expects this field to exist or it fails
  // to build the decode table.
  field bits<32> SoftFail = 0;
  let Size = 4;

  bits<5> rs2;
  bits<5> rs1;
  bits<5> rd;

  let Inst{31-25} = 0b0000000;
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = 0b000;
  let Inst{11-7} = rd;
  let Inst{6-0} = OPC_OP.Value;
  let isCommutable = 1;
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
}

def ADDI : Instruction {
  let Namespace = "StubV";
  dag OutOperandList = (outs GPR:$rd);
  // FIXME: This is actually restricted to a 12-bit immediate
  dag InOperandList = (ins GPR:$rs1, i32imm:$imm12);
  let AsmString = "addi\t$rd, $rs1, $imm12";

  field bits<32> Inst;
  // SoftFail is a field the disassembler can use to provide a way for
  // instructions to not match without killing the whole decode process. It is
  // mainly used for ARM, but Tablegen expects this field to exist or it fails
  // to build the decode table.
  field bits<32> SoftFail = 0;
  let Size = 4;

  bits<12> imm12;
  bits<5> rs1;
  bits<5> rd;

  let Inst{31-20} = imm12;
  let Inst{19-15} = rs1;
  let Inst{14-12} = 0b000;
  let Inst{11-7} = rd;
  let Inst{6-0} = OPC_OP_IMM.Value;
  let isCommutable = 1;
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;

  let isReMaterializable = 1;
  let isAsCheapAsAMove = 1;
}

def JALR : Instruction {
  let Namespace = "StubV";
  dag OutOperandList = (outs GPR:$rd);
  // FIXME: This is actually restricted to a 12-bit immediate
  dag InOperandList = (ins GPR:$rs1, i32imm:$imm12);
  let AsmString = "jalr\t$rd, ${imm12}(${rs1})";

  field bits<32> Inst;
  // SoftFail is a field the disassembler can use to provide a way for
  // instructions to not match without killing the whole decode process. It is
  // mainly used for ARM, but Tablegen expects this field to exist or it fails
  // to build the decode table.
  field bits<32> SoftFail = 0;
  let Size = 4;

  bits<5> rs1;
  bits<5> rd;
  bits<12> imm12;

  let Inst{31-20} = imm12;
  let Inst{19-15} = rs1;
  let Inst{14-12} = 0b000;
  let Inst{11-7} = rd;
  let Inst{6-0} = OPC_JALR.Value;

  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
}

//===----------------------------------------------------------------------===//
// Assembler Pseudo Instructions
//===----------------------------------------------------------------------===//

def : InstAlias<"nop",           (ADDI      X0,      X0,       0)>;

def : InstAlias<"li $rd, $imm",  (ADDI GPR:$rd, X0, i32imm:$imm)>;
def : InstAlias<"mv $rd, $rs",   (ADDI GPR:$rd, GPR:$rs,       0)>;

// The GNU (and LLVM) RISC-V assemblers support the reg-reg mnemonic even when
// an immediate is used.
def : InstAlias<"add $rd, $rs1, $imm12",
                (ADDI  GPR:$rd, GPR:$rs1, i32imm:$imm12)>;

def : InstAlias<"ret",                   (JALR      X0,      X1, 0), 4>;

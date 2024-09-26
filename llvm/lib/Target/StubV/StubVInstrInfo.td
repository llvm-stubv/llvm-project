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

def : InstAlias<"ret",                   (JALR      X0,      X1, 0), 4>;


def XOR : Instruction {
  let Namespace = "StubV";
  dag OutOperandList = (outs GPR:$rd);
  dag InOperandList = (ins GPR:$rs1, GPR:$rs2);
  let AsmString = "xor\t$rd, $rs1, $rs2";

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

  let Inst{6-0}   = 0b0110011;  // First 6 Bits seems to signify which class the instruction belongs to (from the RISCV manual)
  let Inst{11-7}  = rd;        // The output register
  let Inst{14-12} = 0b100;     // 3 More bits to point to the XOR instruction, the first 6 bits maybe common for diff instrs.
  let Inst{19-15} = rs1;       // Operand number 1
  let Inst{20-24} = rs2;       // Operand number 2
  let Inst{31-25} = 0b0000000; // Fill the remaining bits with 0, as the RISCV manual says

  let hasSideEffects = 0; // This instr does not raise any exceptions, traps: e.g division by 0
  let mayLoad  = 0; // This instr will never load from memory
  let mayStore = 0; // This instr will never store to memory
  let isCommutable = 1;
}

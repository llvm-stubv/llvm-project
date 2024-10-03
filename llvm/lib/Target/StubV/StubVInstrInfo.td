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

def stubv_ret_glue  : SDNode<"StubVISD::RET_GLUE", SDTNone,
                             [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

//===----------------------------------------------------------------------===//
// Instruction Formats
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

def : Pat<(XLenVT (add (XLenVT GPR:$rs1), (XLenVT GPR:$rs2))),
          (ADD GPR:$rs1, GPR:$rs2)>;
// FIXME: This should be restricted to 12 bit immediates.
def : Pat<(XLenVT (add (XLenVT GPR:$rs1), imm:$simm12)),
          (ADDI GPR:$rs1, imm:$simm12)>;

def OR : Instruction {
  let Namespace = "StubV";
  dag OutOperandList = (outs GPR:$rd);
  dag InOperandList = (ins GPR:$rs1, GPR:$rs2);
  let AsmString = "or\t$rd, $rs1, $rs2";

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
  let Inst{14-12} = 0b110;
  let Inst{11-7} = rd;
  let Inst{6-0} = OPC_OP.Value;
  let isCommutable = 1;
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
}

def SRA : Instruction {
  let Namespace = "StubV";
  dag OutOperandList = (outs GPR:$rd);
  dag InOperandList = (ins GPR:$rs1, GPR:$rs2);
  let AsmString = "sra\t$rd, $rs1, $rs2";

  field bits<32> Inst;
  field bits<32> SoftFail = 0;
  let Size = 4;

  bits<5> rs2;
  bits<5> rs1;
  bits<5> rd;

  let Inst{31-25} = 0b0100000;
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = 0b101;
  let Inst{11-7} = rd;
  let Inst{6-0} = OPC_OP.Value;

  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
}

def SUB : Instruction {
  let Namespace = "StubV";
  dag OutOperandList = (outs GPR:$rd);
  dag InOperandList = (ins GPR:$rs1, GPR:$rs2);
  let AsmString = "sub\t$rd, $rs1, $rs2";

  field bits<32> Inst;
  field bits<32> SoftFail = 0;
  let Size = 4;

  bits<5> rs2;
  bits<5> rs1;
  bits<5> rd;

  let Inst{31-25} = 0b0100000;
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = 0b000;
  let Inst{11-7} = rd;
  let Inst{6-0} = OPC_OP.Value;
  let isCommutable = 0;
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

def PseudoRET : Instruction,
                PseudoInstExpansion<(JALR X0, X1, 0)> {
  let Namespace = "StubV";
  dag OutOperandList = (outs);
  dag InOperandList = (ins);

  let Pattern = [(stubv_ret_glue)];

  field bits<32> Inst;
  // SoftFail is a field the disassembler can use to provide a way for
  // instructions to not match without killing the whole decode process. It is
  // mainly used for ARM, but Tablegen expects this field to exist or it fails
  // to build the decode table.
  field bits<32> SoftFail = 0;
  let Size = 4;

  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;

  let isBarrier = 1;
  let isReturn = 1;
  let isTerminator = 1;
  let isPseudo = 1;
  let isCodeGenOnly = 1;
}

def SLT : Instruction {
  let Namespace = "StubV";

  dag OutOperandList = (outs GPR:$rd);

  dag InOperandList = (ins GPR:$rs1, GPR:$rs2);

  let AsmString = "slt\t$rd, $rs1, $rs2";

  field bits<32> Inst;
  field bits<32> SoftFail = 0;
  let Size = 4;

  bits<5> rs2;
  bits<5> rs1;
  bits<5> rd;

  let Inst{31-25} = 0b0000000;
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = 0b010;
  let Inst{11-7} = rd;
  let Inst{6-0} = OPC_OP.Value;

  let isCommutable = 1;
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
}

def AND : Instruction {
  let Namespace = "StubV";

  dag OutOperandList = (outs GPR:$rd);
  dag InOperandList = (ins GPR:$rs1, GPR:$rs2);
  let AsmString = "and\t$rd, $rs1, $rs2";

  field bits<32> Inst;
  field bits<32> SoftFail = 0;
  let Size = 4;

  bits<5> rs2;
  bits<5> rs1;
  bits<5> rd;

  let Inst{31-25} = 0b0000000;
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = 0b111;
  let Inst{11-7} = rd;
  let Inst{6-0} = OPC_OP.Value;
  let isCommutable = 1;
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
}

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

def SLL : Instruction {
  let Namespace = "StubV";
  dag OutOperandList = (outs GPR:$rd);
  dag InOperandList = (ins GPR:$rs1, GPR:$rs2);

  let AsmString = "sll\t$rd, ${rs1}, ${rs2}";

  field bits<32> Inst;
  field bits<32> SoftFail = 0;
  let Size = 4;

  bits<5> rd;
  bits<5> rs1;
  bits<5> rs2;

  let Inst{31-25} = 0b0000000;
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = 0b001;
  let Inst{11-7} = rd;
  let Inst{6-0} = OPC_OP.Value;

  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
}

// WIP: I don't know how this all works
def SRL : Instruction {
  let Namespace = "StubV";
  dag OutOperandList = (outs GPR:$rd); // Does this define rd, or is this a
                               // register type that is defined elsewhere?
  dag InOperandList = (ins GPR:$rs1, GPR:$rs2); // Likewise

  let AsmString = "srl\t$rd, ${rs1}, ${rs2}";

  field bits<32> Inst;
  field bits<32> SoftFail = 0;
  let Size = 4;

  bits<5> rd;
  bits<5> rs1;
  bits<5> rs2;

  let Inst{31-25} = 0b0000000;
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = 0b011; // I have chosen this number out of trial and error.
                           // Where are these opcodes specified?
  let Inst{11-7} = rd;
  let Inst{6-0} = OPC_OP.Value; // How is this different to Inst{14-12}. Where
                                // is OPC_OP defined? Is OPC_OP like a struct?
                                // What other members can be used?

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

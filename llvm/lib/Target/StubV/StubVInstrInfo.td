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
// Operand and SDNode transformation definitions.
//===----------------------------------------------------------------------===//

class ImmAsmOperand<string prefix, int width, string suffix> : AsmOperandClass {
  let Name = prefix # "Imm" # width # suffix;
  let RenderMethod = "addImmOperands";
  let DiagnosticType = !strconcat("Invalid", Name);
}

class SImmAsmOperand<int width, string suffix = "">
    : ImmAsmOperand<"S", width, suffix> {
}

class StubVOp<ValueType vt = XLenVT> : Operand<vt> {
  let OperandNamespace = "StubVOp";
}

class StubVSImmOp<int bitsNum> : StubVOp {
  let ParserMatchClass = SImmAsmOperand<bitsNum>;
  let EncoderMethod = "getImmOpValue";
  let DecoderMethod = "decodeSImmOperand<" # bitsNum # ">";
  let OperandType = "OPERAND_SIMM" # bitsNum;
}

class StubVSImmLeafOp<int bitsNum> :
  StubVSImmOp<bitsNum>, ImmLeaf<XLenVT, "return isInt<" # bitsNum # ">(Imm);">;

def simm12 : StubVSImmLeafOp<12> {
  let MCOperandPredicate = [{
    int64_t Imm;
    if (MCOp.evaluateAsConstantImm(Imm))
      return isInt<12>(Imm);
    return MCOp.isBareSymbolRef();
  }];
}

class MemOperand<RegisterClass regClass> : RegisterOperand<regClass>{
  let OperandType = "OPERAND_MEMORY";
}

def GPRMem : MemOperand<GPR>;

// Addressing modes.
def AddrRegImm : ComplexPattern<iPTR, 2, "SelectAddrRegImm">;

//===----------------------------------------------------------------------===//
// Instruction Formats
//===----------------------------------------------------------------------===//

include "StubVInstrFormats.td"

let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
class ALU_rr<bits<7> funct7, bits<3> funct3, string opcodestr,
             bit Commutable = 0>
    : StubVInstR<funct7, funct3, OPC_OP, (outs GPR:$rd), (ins GPR:$rs1, GPR:$rs2),
                 opcodestr, "$rd, $rs1, $rs2"> {
  let isCommutable = Commutable;
}

let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
class ALU_ri<bits<3> funct3, string opcodestr>
    : StubVInstI<funct3, OPC_OP_IMM, (outs GPR:$rd), (ins GPR:$rs1, simm12:$imm12),
                 opcodestr, "$rd, $rs1, $imm12">;

def ADD  : ALU_rr<0b0000000, 0b000, "add", Commutable=1>;
def SUB  : ALU_rr<0b0100000, 0b000, "sub">;

def SLL  : ALU_rr<0b0000000, 0b001, "sll">;
def SRA  : ALU_rr<0b0100000, 0b101, "sra">;
def SRL  : ALU_rr<0b0000000, 0b101, "srl">;

def SLT  : ALU_rr<0b0000000, 0b010, "slt">;

def XOR  : ALU_rr<0b0000000, 0b100, "xor", Commutable=1>;
def OR   : ALU_rr<0b0000000, 0b110, "or", Commutable=1>;
def AND  : ALU_rr<0b0000000, 0b111, "and", Commutable=1>;

// ADDI isn't always rematerializable, but isReMaterializable will be used as
// a hint which is verified in isReallyTriviallyReMaterializable.
let isReMaterializable = 1, isAsCheapAsAMove = 1 in
def ADDI  : ALU_ri<0b000, "addi">;

def ANDI  : ALU_ri<0b111, "andi">;

let isReMaterializable = 1, isAsCheapAsAMove = 1 in {
def XORI  : ALU_ri<0b100, "xori">;
}

def : Pat<(XLenVT (add (XLenVT GPR:$rs1), (XLenVT GPR:$rs2))),
          (ADD GPR:$rs1, GPR:$rs2)>;
def : Pat<(XLenVT (add (XLenVT GPR:$rs1), simm12:$simm12)),
          (ADDI GPR:$rs1, simm12:$simm12)>;

def : Pat<(XLenVT (sub (XLenVT GPR:$rs1), (XLenVT GPR:$rs2))),
          (SUB GPR:$rs1, GPR:$rs2)>;

def : Pat<(XLenVT (and (XLenVT GPR:$rs1), (XLenVT GPR:$rs2))),
          (AND GPR:$rs1, GPR:$rs2)>;
def : Pat<(XLenVT (and (XLenVT GPR:$rs1), simm12:$simm12)),
          (ANDI GPR:$rs1, simm12:$simm12)>;

def : Pat<(XLenVT (xor (XLenVT GPR:$rs1), (XLenVT GPR:$rs2))),
          (XOR GPR:$rs1, GPR:$rs2)>;
def : Pat<(XLenVT (xor (XLenVT GPR:$rs1), simm12:$simm12)),
          (XORI GPR:$rs1, simm12:$simm12)>;


def JALR : Instruction {
  let Namespace = "StubV";
  dag OutOperandList = (outs GPR:$rd);
  dag InOperandList = (ins GPR:$rs1, simm12:$imm12);
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

def LW : Instruction {
  let Namespace = "StubV";
  dag OutOperandList = (outs GPR:$rd);
  dag InOperandList = (ins GPRMem:$rs1, simm12:$imm12);

  let AsmString = "lw\t$rd, ${imm12}(${rs1})";

  field bits<32> Inst;
  field bits<32> SoftFail = 0;
  let Size = 4;

  bits<12> imm12;
  bits<5> rd;
  bits<5> rs1;

  let Inst{31-20} = imm12;
  let Inst{19-15} = rs1;
  let Inst{14-12} = 0b010;
  let Inst{11-7} = rd;
  let Inst{6-0} = OPC_LOAD.Value;

  let hasSideEffects = 0;
  let mayLoad = 1;
  let mayStore = 0;
}

def : Pat<(XLenVT (load (AddrRegImm (XLenVT GPR:$rs1), simm12:$imm12))),
          (LW GPR:$rs1, simm12:$imm12)>;

def SW : Instruction {
  let Namespace = "StubV";
  dag OutOperandList = (outs);
  dag InOperandList = (ins GPR:$rs2, GPRMem:$rs1, simm12:$imm12);

  let AsmString = "sw\t$rs2, ${imm12}(${rs1})";

  field bits<32> Inst;
  field bits<32> SoftFail = 0;
  let Size = 4;

  bits<12> imm12;
  bits<5> rs2;
  bits<5> rs1;

  let Inst{31-25} = imm12{11-5};
  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = 0b010;
  let Inst{11-7} = imm12{4-0};
  let Inst{6-0} = OPC_STORE.Value;

  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 1;
}

def : Pat<(store (XLenVT GPR:$rs2), (AddrRegImm (XLenVT GPR:$rs1), simm12:$imm12)),
          (SW GPR:$rs2, GPR:$rs1, simm12:$imm12)>;

def SRLI : Instruction {
  let Namespace = "StubV";
  dag OutOperandList = (outs GPR:$rd);
  dag InOperandList = (ins GPR:$rs1, simm12:$imm12);

  let AsmString = "srl\t${rd}, ${rs1}, ${imm12}";

  field bits<32> Inst;
  field bits<32> SoftFail = 0;
  let Size = 4;

  bits<12> imm12;
  bits<5> rs1;
  bits<5> rd;

  let Inst{31-20} = imm12;
  let Inst{19-15} = rs1;
  let Inst{14-12} = 0b101;
  let Inst{11-7} = rd;
  let Inst{6-0} = OPC_OP.Value;

  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
}

def : Pat<(XLenVT (srl (XLenVT GPR:$rs1), (XLenVT GPR:$rs2))),
          (SRL GPR:$rs1, GPR:$rs2)>;
def : Pat<(XLenVT (srl (XLenVT GPR:$rs1), simm12:$imm12)),
          (SRLI GPR:$rs1, simm12:$imm12)>;

//===----------------------------------------------------------------------===//
// Assembler Pseudo Instructions
//===----------------------------------------------------------------------===//

def : InstAlias<"nop",           (ADDI      X0,      X0,       0)>;

def : InstAlias<"li $rd, $imm",  (ADDI GPR:$rd, X0, simm12:$imm)>;
def : InstAlias<"mv $rd, $rs",   (ADDI GPR:$rd, GPR:$rs,       0)>;
def : InstAlias<"not $rd, $rs",  (XORI GPR:$rd, GPR:$rs,      -1)>;

// The GNU (and LLVM) RISC-V assemblers support the reg-reg mnemonic even when
// an immediate is used.
def : InstAlias<"add $rd, $rs1, $imm12",
                (ADDI  GPR:$rd, GPR:$rs1, simm12:$imm12)>;
def : InstAlias<"and $rd, $rs1, $imm12",
                (ANDI  GPR:$rd, GPR:$rs1, simm12:$imm12)>;
def : InstAlias<"xor $rd, $rs1, $imm12",
                (XORI  GPR:$rd, GPR:$rs1, simm12:$imm12)>;

def : InstAlias<"lw $rd, (${rs1})",
                (LW  GPR:$rd, GPR:$rs1, 0)>;

def : InstAlias<"sw $rs2, (${rs1})",
                (SW  GPR:$rs2, GPR:$rs1, 0)>;

def : InstAlias<"ret",                   (JALR      X0,      X1, 0), 4>;

//===----------------------------------------------------------------------===//
// Standard extensions
//===----------------------------------------------------------------------===//

// Multiply and Division
include "StubVInstrInfoM.td"

//===-- StubVInstrFormats.td - Stub-V Instruction Formats --*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// The following opcode names match those given in Table 19.1 in the
// RISC-V User-level ISA specification ("RISC-V base opcode map").
class StubVOpcode<string name, bits<7> val> {
  string Name = name;
  bits<7> Value = val;
}

def OPC_LOAD      : StubVOpcode<"LOAD",      0b0000011>;
def OPC_STORE     : StubVOpcode<"STORE",     0b0100011>;
def OPC_OP        : StubVOpcode<"OP",        0b0110011>;
def OPC_OP_IMM    : StubVOpcode<"OP_IMM",    0b0010011>;
def OPC_JALR      : StubVOpcode<"JALR",      0b1100111>;

class StubVInstCommon<dag outs, dag ins, string opcodestr, string argstr,
                      list<dag> pattern> : Instruction {
  let Namespace = "StubV";

  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = opcodestr # !if(!empty(argstr), "", "\t" # argstr);
  let Pattern = pattern;
}

class StubVInst<dag outs, dag ins, string opcodestr, string argstr,
                list<dag> pattern>
    : StubVInstCommon<outs, ins, opcodestr, argstr, pattern> {
  field bits<32> Inst;
  // SoftFail is a field the disassembler can use to provide a way for
  // instructions to not match without killing the whole decode process. It is
  // mainly used for ARM, but Tablegen expects this field to exist or it fails
  // to build the decode table.
  field bits<32> SoftFail = 0;
  let Size = 4;
}

// Common base class for R format instructions. Bits {31-25} should be set by
// the subclasses.
class StubVInstRBase<bits<3> funct3, StubVOpcode opcode, dag outs,
                     dag ins, string opcodestr, string argstr>
    : StubVInst<outs, ins, opcodestr, argstr, []> {
  bits<5> rs2;
  bits<5> rs1;
  bits<5> rd;

  let Inst{24-20} = rs2;
  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-7} = rd;
  let Inst{6-0} = opcode.Value;
}

class StubVInstR<bits<7> funct7, bits<3> funct3, StubVOpcode opcode, dag outs,
                 dag ins, string opcodestr, string argstr>
    : StubVInstRBase<funct3, opcode, outs, ins, opcodestr, argstr> {
  let Inst{31-25} = funct7;
}

// Common base class for I format instructions. Bits {31-20} should be set by
// the subclasses.
class StubVInstIBase<bits<3> funct3, StubVOpcode opcode, dag outs, dag ins,
                     string opcodestr, string argstr>
    : StubVInst<outs, ins, opcodestr, argstr, []> {
  bits<5> rs1;
  bits<5> rd;

  let Inst{19-15} = rs1;
  let Inst{14-12} = funct3;
  let Inst{11-7} = rd;
  let Inst{6-0} = opcode.Value;
}

class StubVInstI<bits<3> funct3, StubVOpcode opcode, dag outs, dag ins,
                 string opcodestr, string argstr>
    : StubVInstIBase<funct3, opcode, outs, ins, opcodestr, argstr> {
  bits<12> imm12;

  let Inst{31-20} = imm12;
}

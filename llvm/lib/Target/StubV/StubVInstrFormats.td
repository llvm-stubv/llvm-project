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

def OPC_OP        : StubVOpcode<"OP",        0b0110011>;
def OPC_JALR      : StubVOpcode<"JALR",      0b1100111>;

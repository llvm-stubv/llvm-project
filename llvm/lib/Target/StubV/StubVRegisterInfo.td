//===-- StubVRegisterInfo.td - Stub-V Register defs --------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the Stub-V register files
//===----------------------------------------------------------------------===//


let Namespace = "StubV" in {

class StubVReg<bits<5> Enc, string n, list<string> alt = []> : Register<n> {
  let HWEncoding{4-0} = Enc;
  let AltNames = alt;
}

let FallbackRegAltNameIndex = NoRegAltName in
def ABIRegAltName : RegAltNameIndex;

} // Namespace "StubV"

let RegAltNameIndices = [ABIRegAltName] in {
  let isConstant = true in
  def X0  : StubVReg<0, "x0", ["zero"]>, DwarfRegNum<[0]>;
  def X1  : StubVReg<1, "x1", ["ra"]>, DwarfRegNum<[1]>;
  def X2  : StubVReg<2, "x2", ["sp"]>, DwarfRegNum<[2]>;
  def X3  : StubVReg<3, "x3", ["gp"]>, DwarfRegNum<[3]>;
  def X4  : StubVReg<4, "x4", ["tp"]>, DwarfRegNum<[4]>;
  def X5  : StubVReg<5, "x5", ["t0"]>, DwarfRegNum<[5]>;
  def X6  : StubVReg<6, "x6", ["t1"]>, DwarfRegNum<[6]>;
  def X7  : StubVReg<7, "x7", ["t2"]>, DwarfRegNum<[7]>;
  def X8  : StubVReg<8, "x8", ["s0", "fp"]>, DwarfRegNum<[8]>;
  def X9  : StubVReg<9, "x9", ["s1"]>, DwarfRegNum<[9]>;
  def X10 : StubVReg<10,"x10", ["a0"]>, DwarfRegNum<[10]>;
  def X11 : StubVReg<11,"x11", ["a1"]>, DwarfRegNum<[11]>;
  def X12 : StubVReg<12,"x12", ["a2"]>, DwarfRegNum<[12]>;
  def X13 : StubVReg<13,"x13", ["a3"]>, DwarfRegNum<[13]>;
  def X14 : StubVReg<14,"x14", ["a4"]>, DwarfRegNum<[14]>;
  def X15 : StubVReg<15,"x15", ["a5"]>, DwarfRegNum<[15]>;
  def X16 : StubVReg<16,"x16", ["a6"]>, DwarfRegNum<[16]>;
  def X17 : StubVReg<17,"x17", ["a7"]>, DwarfRegNum<[17]>;
  def X18 : StubVReg<18,"x18", ["s2"]>, DwarfRegNum<[18]>;
  def X19 : StubVReg<19,"x19", ["s3"]>, DwarfRegNum<[19]>;
  def X20 : StubVReg<20,"x20", ["s4"]>, DwarfRegNum<[20]>;
  def X21 : StubVReg<21,"x21", ["s5"]>, DwarfRegNum<[21]>;
  def X22 : StubVReg<22,"x22", ["s6"]>, DwarfRegNum<[22]>;
  def X23 : StubVReg<23,"x23", ["s7"]>, DwarfRegNum<[23]>;
  def X24 : StubVReg<24,"x24", ["s8"]>, DwarfRegNum<[24]>;
  def X25 : StubVReg<25,"x25", ["s9"]>, DwarfRegNum<[25]>;
  def X26 : StubVReg<26,"x26", ["s10"]>, DwarfRegNum<[26]>;
  def X27 : StubVReg<27,"x27", ["s11"]>, DwarfRegNum<[27]>;
  def X28 : StubVReg<28,"x28", ["t3"]>, DwarfRegNum<[28]>;
  def X29 : StubVReg<29,"x29", ["t4"]>, DwarfRegNum<[29]>;
  def X30 : StubVReg<30,"x30", ["t5"]>, DwarfRegNum<[30]>;
  def X31 : StubVReg<31,"x31", ["t6"]>, DwarfRegNum<[31]>;
}

class StubVRegisterClass<list<ValueType> regTypes, int align, dag regList>
    : RegisterClass<"StubV", regTypes, align, regList> {
}

// Only supporting 32-bit for now
defvar XLenVT = i32;

class GPRRegisterClass<dag regList>
    : StubVRegisterClass<[XLenVT], 32, regList> {
}

// The order of registers represents the preferred allocation sequence.
// Registers are listed in the order caller-save, callee-save, specials.
def GPR : GPRRegisterClass<(add (sequence "X%u", 10, 17),
                                (sequence "X%u", 5, 7),
                                (sequence "X%u", 28, 31),
                                (sequence "X%u", 8, 9),
                                (sequence "X%u", 18, 27),
                                (sequence "X%u", 0, 4))>;

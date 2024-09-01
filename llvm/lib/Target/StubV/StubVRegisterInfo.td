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

} // Namespace "StubV"

  let isConstant = true in
  def X0  : StubVReg<0, "x0", ["zero"]>;
  def X1  : StubVReg<1, "x1", ["ra"]>;
  def X2  : StubVReg<2, "x2", ["sp"]>;
  def X3  : StubVReg<3, "x3", ["gp"]>;
  def X4  : StubVReg<4, "x4", ["tp"]>;
  def X5  : StubVReg<5, "x5", ["t0"]>;
  def X6  : StubVReg<6, "x6", ["t1"]>;
  def X7  : StubVReg<7, "x7", ["t2"]>;
  def X8  : StubVReg<8, "x8", ["s0", "fp"]>;
  def X9  : StubVReg<9, "x9", ["s1"]>;
  def X10 : StubVReg<10,"x10", ["a0"]>;
  def X11 : StubVReg<11,"x11", ["a1"]>;
  def X12 : StubVReg<12,"x12", ["a2"]>;
  def X13 : StubVReg<13,"x13", ["a3"]>;
  def X14 : StubVReg<14,"x14", ["a4"]>;
  def X15 : StubVReg<15,"x15", ["a5"]>;
  def X16 : StubVReg<16,"x16", ["a6"]>;
  def X17 : StubVReg<17,"x17", ["a7"]>;
  def X18 : StubVReg<18,"x18", ["s2"]>;
  def X19 : StubVReg<19,"x19", ["s3"]>;
  def X20 : StubVReg<20,"x20", ["s4"]>;
  def X21 : StubVReg<21,"x21", ["s5"]>;
  def X22 : StubVReg<22,"x22", ["s6"]>;
  def X23 : StubVReg<23,"x23", ["s7"]>;
  def X24 : StubVReg<24,"x24", ["s8"]>;
  def X25 : StubVReg<25,"x25", ["s9"]>;
  def X26 : StubVReg<26,"x26", ["s10"]>;
  def X27 : StubVReg<27,"x27", ["s11"]>;
  def X28 : StubVReg<28,"x28", ["t3"]>;
  def X29 : StubVReg<29,"x29", ["t4"]>;
  def X30 : StubVReg<30,"x30", ["t5"]>;
  def X31 : StubVReg<31,"x31", ["t6"]>;

class StubVRegisterClass<list<ValueType> regTypes, int align, dag regList>
    : RegisterClass<"StubV", regTypes, align, regList> {
}

class GPRRegisterClass<dag regList>
    : StubVRegisterClass<[i32], 32, regList> {
}

// The order of registers represents the preferred allocation sequence.
// Registers are listed in the order caller-save, callee-save, specials.
def GPR : GPRRegisterClass<(add (sequence "X%u", 10, 17),
                                (sequence "X%u", 5, 7),
                                (sequence "X%u", 28, 31),
                                (sequence "X%u", 8, 9),
                                (sequence "X%u", 18, 27),
                                (sequence "X%u", 0, 4))>;

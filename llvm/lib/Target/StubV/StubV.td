//===-- StubV.td - Describe the Stub-V Target Machine ------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Registers, calling conventions, instruction descriptions.
//===----------------------------------------------------------------------===//

include "StubVRegisterInfo.td"
include "StubVInstrInfo.td"

def StubVInstrInfo : InstrInfo {
  let guessInstructionProperties = 0;
}

def StubVAsmParser : AsmParser {
  let ShouldEmitMatchRegisterAltName = 1;
  let AllowDuplicateRegisterNames = 1;
}

def StubVAsmWriter : AsmWriter {
  int PassSubtarget = 0;
}

class Proc<string Name, list<SubtargetFeature> Features,
           list<SubtargetFeature> TuneFeatures = []>
 : Processor<Name, NoItineraries, Features, TuneFeatures>;

def : Proc<"generic",         []>;

def StubV : Target {
  let InstructionSet = StubVInstrInfo;
  let AssemblyParsers = [StubVAsmParser];
  let AssemblyWriters = [StubVAsmWriter];
}

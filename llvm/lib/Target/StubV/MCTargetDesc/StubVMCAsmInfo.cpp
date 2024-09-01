//===-- StubVMCAsmInfo.cpp - Stub-V Asm properties ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the StubVMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "StubVMCAsmInfo.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/TargetParser/Triple.h"
using namespace llvm;

void StubVMCAsmInfo::anchor() {}

StubVMCAsmInfo::StubVMCAsmInfo(const Triple &TT) {
  CodePointerSize = CalleeSaveStackSlotSize = 4;
  CommentString = "#";
  AlignmentIsInBytes = false;
  SupportsDebugInformation = true;
  ExceptionsType = ExceptionHandling::DwarfCFI;
  Data16bitsDirective = "\t.half\t";
  Data32bitsDirective = "\t.word\t";
}

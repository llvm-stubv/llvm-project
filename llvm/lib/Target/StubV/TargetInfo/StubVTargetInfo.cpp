//===-- StubVTargetInfo.cpp - Stub-V Target Implementation ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/StubVTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

Target &llvm::getTheStubV32Target() {
  static Target TheStubV32Target;
  return TheStubV32Target;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeStubVTargetInfo() {
  RegisterTarget<Triple::stubv32, /*HasJIT=*/false> X(
      getTheStubV32Target(), "stubv32", "32-bit Stub-V", "StubV");
}

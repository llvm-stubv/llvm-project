//===-- StubVISelLowering.h - Stub-V DAG Lowering Interface -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Stub-V uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_STUBV_STUBVISELLOWERING_H
#define LLVM_LIB_TARGET_STUBV_STUBVISELLOWERING_H

#include "StubV.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

class StubVSubtarget;

class StubVTargetLowering : public TargetLowering {
  const StubVSubtarget &Subtarget;

public:
  explicit StubVTargetLowering(const TargetMachine &TM,
                               const StubVSubtarget &STI);
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_STUBV_STUBVISELLOWERING_H

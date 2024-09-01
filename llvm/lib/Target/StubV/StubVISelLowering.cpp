//===-- StubVISelLowering.cpp - Stub-V DAG Lowering Implementation  -------===//
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

#include "StubVISelLowering.h"
#include "StubV.h"
#include "StubVSubtarget.h"
#include "StubVTargetMachine.h"

using namespace llvm;

#define DEBUG_TYPE "stubv-lower"

StubVTargetLowering::StubVTargetLowering(const TargetMachine &TM,
                                         const StubVSubtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {}

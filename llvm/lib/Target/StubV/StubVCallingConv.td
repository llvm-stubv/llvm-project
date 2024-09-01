//===-- StubVCallingConv.td - Calling Conventions Stub-V ---*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the Stub-V architecture.
//
//===----------------------------------------------------------------------===//

def CSR_ILP32E_LP64E : CalleeSavedRegs<(add X1, X8, X9)>;

def CSR_ILP32_LP64
    : CalleeSavedRegs<(add CSR_ILP32E_LP64E, (sequence "X%u", 18, 27))>;

# RUN: not llvm-mc --triple stubv32 < %s -show-inst -o /dev/null 2>&1 | FileCheck %s

and a0, a1, 2050   # CHECK: :[[@LINE]]:13: error: immediate must be an integer in the range [-2048, 2047]
andi x0, x0, 2051 # CHECK: :[[@LINE]]:14: error: immediate must be an integer in the range [-2048, 2047]

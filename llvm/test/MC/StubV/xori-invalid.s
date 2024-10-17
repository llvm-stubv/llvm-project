# RUN: not llvm-mc --triple stubv32 < %s -show-inst -o /dev/null 2>&1 | FileCheck %s

xor a0, a1, 2048 # CHECK: :[[@LINE]]:13: error: immediate must be an integer in the range [-2048, 2047]
xori x0, x0, 2048 # CHECK: :[[@LINE]]:14: error: immediate must be an integer in the range [-2048, 2047]

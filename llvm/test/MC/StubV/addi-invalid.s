# RUN: not llvm-mc --triple stubv32 < %s -show-inst -o /dev/null 2>&1 | FileCheck %s

add a0, a1, 3000   # CHECK: :[[@LINE]]:13: error: immediate must be an integer in the range [-2048, 2047]
addi x0, x0, 20000 # CHECK: :[[@LINE]]:14: error: immediate must be an integer in the range [-2048, 2047]
li s0, 2049        # CHECK: :[[@LINE]]:8: error: immediate must be an integer in the range [-2048, 2047]

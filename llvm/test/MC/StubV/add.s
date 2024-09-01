# RUN: llvm-mc --triple stubv32 < %s

add a0, a1, a2
ret
jalr x0, 0(x1)

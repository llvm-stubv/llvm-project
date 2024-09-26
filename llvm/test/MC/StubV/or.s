# RUN: llvm-mc --triple stubv32 < %s -show-inst | FileCheck %s --check-prefix CHECK-INST
# RUN: llvm-mc --triple stubv32 < %s -show-encoding | FileCheck %s --check-prefix CHECK-ENCODING

# 0b0000000 (func7) 01100 (rs2) 01011 (rs1) 110 (funct3) 01010 (rd) 0110011 (opcode)
# 0b00000000 11000101 11100101 00110011
# 0x00       c5       e5       33
# CHECK-ENCODING: encoding: [0x33,0xe5,0xc5,0x00]
# CHECK-INST:      <MCInst #{{[0-9]+}} OR
# CHECK-INST-NEXT:  <MCOperand Reg:11>
# CHECK-INST-NEXT:  <MCOperand Reg:12>
# CHECK-INST-NEXT:  <MCOperand Reg:13>>
or a0, a1, a2

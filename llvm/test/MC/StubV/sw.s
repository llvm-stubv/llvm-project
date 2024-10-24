# RUN: llvm-mc --triple stubv32 < %s -show-inst | FileCheck %s --check-prefix CHECK-INST
# RUN: llvm-mc --triple stubv32 < %s -show-encoding | FileCheck %s --check-prefix CHECK-ENCODING

# CHECK-ENCODING: encoding: [0x23,0x22,0xd7,0x00]
# CHECK-INST:      <MCInst #{{[0-9]+}} SW
# CHECK-INST-NEXT:  <MCOperand Reg:14>
# CHECK-INST-NEXT:  <MCOperand Reg:15>
# CHECK-INST-NEXT:  <MCOperand Imm:4>>
sw a3, 4(a4)

# CHECK-ENCODING: encoding: [0x23,0xa0,0x08,0x00]
# CHECK-INST:      <MCInst #{{[0-9]+}} SW
# CHECK-INST-NEXT:  <MCOperand Reg:1>
# CHECK-INST-NEXT:  <MCOperand Reg:18>
# CHECK-INST-NEXT:  <MCOperand Imm:0>>
sw zero, (a7)

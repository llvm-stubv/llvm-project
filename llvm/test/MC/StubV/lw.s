# RUN: llvm-mc --triple stubv32 < %s -show-inst | FileCheck %s --check-prefix CHECK-INST
# RUN: llvm-mc --triple stubv32 < %s -show-encoding | FileCheck %s --check-prefix CHECK-ENCODING

# CHECK-ENCODING: encoding: [0x03,0xa7,0x87,0x00]
# CHECK-INST:      <MCInst #{{[0-9]+}} LW
# CHECK-INST-NEXT:  <MCOperand Reg:15>
# CHECK-INST-NEXT:  <MCOperand Reg:16>
# CHECK-INST-NEXT:  <MCOperand Imm:8>>
lw a4, 8(a5)

# CHECK-ENCODING: encoding: [0x03,0xa0,0x08,0x00]
# CHECK-INST:      <MCInst #{{[0-9]+}} LW
# CHECK-INST-NEXT:  <MCOperand Reg:1>
# CHECK-INST-NEXT:  <MCOperand Reg:18>
# CHECK-INST-NEXT:  <MCOperand Imm:0>>
lw zero, (a7)

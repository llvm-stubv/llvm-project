# RUN: llvm-mc --triple stubv32 < %s -show-inst | FileCheck %s --check-prefix CHECK-INST
# RUN: llvm-mc --triple stubv32 < %s -show-encoding | FileCheck %s --check-prefix CHECK-ENCODING

# CHECK-ENCODING: encoding: [0x33,0x85,0xc5,0x00]
# CHECK-INST:      <MCInst #{{[0-9]+}} ADD
# CHECK-INST-NEXT:  <MCOperand Reg:11>
# CHECK-INST-NEXT:  <MCOperand Reg:12>
# CHECK-INST-NEXT:  <MCOperand Reg:13>>
add a0, a1, a2

# CHECK-ENCODING: encoding: [0x13,0x85,0x25,0x00]
# CHECK-INST:      <MCInst #{{[0-9]+}} ADDI
# CHECK-INST-NEXT:  <MCOperand Reg:11>
# CHECK-INST-NEXT:  <MCOperand Reg:12>
# CHECK-INST-NEXT:  <MCOperand Imm:2>>
add a0, a1, 2

# CHECK-ENCODING: encoding: [0x13,0x00,0x00,0x00]
# CHECK-INST:      <MCInst #{{[0-9]+}} ADDI
# CHECK-INST-NEXT:  <MCOperand Reg:1>
# CHECK-INST-NEXT:  <MCOperand Reg:1>
# CHECK-INST-NEXT:  <MCOperand Imm:0>>
nop

# CHECK-ENCODING: encoding: [0x13,0x00,0x00,0x00]
# CHECK-INST:      <MCInst #{{[0-9]+}} ADDI
# CHECK-INST-NEXT:  <MCOperand Reg:1>
# CHECK-INST-NEXT:  <MCOperand Reg:1>
# CHECK-INST-NEXT:  <MCOperand Imm:0>>
addi x0, x0, 0

# CHECK-ENCODING: encoding: [0x13,0x04,0xa0,0x02]
# CHECK-INST:      <MCInst #{{[0-9]+}} ADDI
# CHECK-INST-NEXT:  <MCOperand Reg:9>
# CHECK-INST-NEXT:  <MCOperand Reg:1>
# CHECK-INST-NEXT:  <MCOperand Imm:42>>
li s0, 42

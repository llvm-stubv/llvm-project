# RUN: llvm-mc --triple stubv32 < %s -show-inst | FileCheck %s --check-prefix CHECK-INST
# RUN: llvm-mc --triple stubv32 < %s -show-encoding | FileCheck %s --check-prefix CHECK-ENCODING

# CHECK-ENCODING: encoding: [0x33,0xc5,0x65,0x00]
# CHECK-INST:      <MCInst #{{.*}} XOR
# CHECK-INST-NEXT:  <MCOperand Reg:11>
# CHECK-INST-NEXT:  <MCOperand Reg:12>
# CHECK-INST-NEXT:  <MCOperand Reg:13>>
xor a0, a1, a2

# CHECK-ENCODING: encoding: [0x13,0xc5,0xc5,0x00]
# CHECK-INST: <MCInst #{{.*}} XORI
# CHECK-INST-NEXT: <MCOperand Reg:11>
# CHECK-INST-NEXT: <MCOperand Reg:12>
# CHECK-INST-NEXT: <MCOperand Imm:12>>
xor a0, a1, 12

# CHECK-ENCONDING: encoding: [0x13,0xc5,0xc5,0x00]
# CHECK-INST: <MCInst #308 XORI
# CHECK-INST-NEXT: <MCOperand Reg:11>
# CHECK-INST-NEXT: <MCOperand Reg:12>
# CHECK-INST-NEXT: <MCOperand Imm:12>>
xori a0, a1, 12

# CHECK-ENCODING: encoding: [0x13,0xc5,0xf5,0xff]
# CHECK-INST: <MCInst #308 XORI
# CHECK-INST-NEXT: <MCOperand Reg:11>
# CHECK-INST-NEXT: <MCOperand Reg:12>
# CHECK-INST-NEXT: <MCOperand Imm:-1>>
not a0, a1

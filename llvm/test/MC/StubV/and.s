# RUN: llvm-mc --triple stubv32 < %s -show-inst | FileCheck %s --check-prefix CHECK-INST
# RUN: llvm-mc --triple stubv32 < %s -show-encoding | FileCheck %s --check-prefix CHECK-ENCODING

# CHECK-ENCODING: encoding: [0x33,0xf5,0xc5,0x00]
# CHECK-INST:      <MCInst #{{[0-9]+}} AND
# CHECK-INST-NEXT:  <MCOperand Reg:11>
# CHECK-INST-NEXT:  <MCOperand Reg:12>
# CHECK-INST-NEXT:  <MCOperand Reg:13>>
and a0, a1, a2

# CHECK-ENCODING: encoding: [0x13,0xf5,0xa5,0x02]
# CHECK-INST:      <MCInst #{{[0-9]+}} ANDI
# CHECK-INST-NEXT:  <MCOperand Reg:11>
# CHECK-INST-NEXT:  <MCOperand Reg:12>
# CHECK-INST-NEXT:  <MCOperand Imm:42>>
andi a0, a1, 42

# RUN: llvm-mc --triple stubv32 < %s -show-inst | FileCheck %s --check-prefix CHECK-INST
# RUN: llvm-mc --triple stubv32 < %s -show-encoding | FileCheck %s --check-prefix CHECK-ENCODING

# CHECK-ENCODING: encoding: [0x67,0x80,0x00,0x00]
# CHECK-INST:      <MCInst #{{[0-9]+}} JALR
# CHECK-INST-NEXT:  <MCOperand Reg:1>
# CHECK-INST-NEXT:  <MCOperand Reg:2>
# CHECK-INST-NEXT:  <MCOperand Imm:0>>
ret
# CHECK-ENCODING: encoding: [0x67,0x80,0x00,0x00]
# CHECK-INST:      <MCInst #{{[0-9]+}} JALR
# CHECK-INST-NEXT:  <MCOperand Reg:1>
# CHECK-INST-NEXT:  <MCOperand Reg:2>
# CHECK-INST-NEXT:  <MCOperand Imm:0>>
jalr x0, 0(x1)

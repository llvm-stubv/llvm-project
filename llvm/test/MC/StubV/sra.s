# RUN: llvm-mc --triple stubv32 < %s -show-inst | FileCheck %s --check-prefix CHECK-INST
# RUN: llvm-mc --triple stubv32 < %s -show-encoding | FileCheck %s --check-prefix CHECK-ENCODING

# CHECK-ENCODING: encoding: [0x33,0xd5,0xc5,0x40]
# CHECK-INST:      <MCInst #{{[0-9]+}} SRA
# CHECK-INST-NEXT:  <MCOperand Reg:11>
# CHECK-INST-NEXT:  <MCOperand Reg:12>
# CHECK-INST-NEXT:  <MCOperand Reg:13>>
sra a0, a1, a2

# RUN: llvm-mc --triple stubv32 < %s -show-inst | FileCheck %s --check-prefix CHECK-INST
# RUN: llvm-mc --triple stubv32 < %s -show-encoding | FileCheck %s --check-prefix CHECK-ENCODING

# CHECK-ENCODING: encoding: [0x33,0x85,0xc5,0x40]
# CHECK-INST:      <MCInst #{{.*}} SUB
# CHECK-INST-NEXT:  <MCOperand Reg:11>
# CHECK-INST-NEXT:  <MCOperand Reg:12>
# CHECK-INST-NEXT:  <MCOperand Reg:13>>
sub a0, a1, a2

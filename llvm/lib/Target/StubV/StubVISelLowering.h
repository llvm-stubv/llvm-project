//===-- StubVISelLowering.h - Stub-V DAG Lowering Interface -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Stub-V uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_STUBV_STUBVISELLOWERING_H
#define LLVM_LIB_TARGET_STUBV_STUBVISELLOWERING_H

#include "MCTargetDesc/StubVBaseInfo.h"
#include "StubV.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

namespace StubVISD {
// clang-format off
enum NodeType : unsigned {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  RET_GLUE,
};
} // namespace StubVISD

class StubVSubtarget;

class StubVTargetLowering : public TargetLowering {
  const StubVSubtarget &Subtarget;

public:
  explicit StubVTargetLowering(const TargetMachine &TM,
                               const StubVSubtarget &STI);

  const StubVSubtarget &getSubtarget() const { return Subtarget; }

  // This method returns the name of a target specific DAG node.
  const char *getTargetNodeName(unsigned Opcode) const override;

  // Lower incoming arguments, copy physregs into vregs
  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;

  // Provide custom lowering hooks for some operations.
  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;
  void ReplaceNodeResults(SDNode *N, SmallVectorImpl<SDValue> &Results,
                          SelectionDAG &DAG) const override;

private:
  void analyzeInputArgs(MachineFunction &MF, CCState &CCInfo,
                        const SmallVectorImpl<ISD::InputArg> &Ins,
                        bool IsRet) const;
  void analyzeOutputArgs(MachineFunction &MF, CCState &CCInfo,
                         const SmallVectorImpl<ISD::OutputArg> &Outs,
                         bool IsRet, CallLoweringInfo *CLI) const;

  SDValue lowerCTPOP(SDValue Op, SelectionDAG &DAG) const;
};

namespace StubV {

bool CC_StubV(const DataLayout &DL, StubVABI::ABI ABI, unsigned ValNo,
              MVT ValVT, MVT LocVT, CCValAssign::LocInfo LocInfo,
              ISD::ArgFlagsTy ArgFlags, CCState &State, bool IsFixed,
              bool IsRet, Type *OrigTy, const StubVTargetLowering &TLI);

ArrayRef<MCPhysReg> getArgGPRs(const StubVABI::ABI ABI);

} // namespace StubV

} // namespace llvm

#endif // LLVM_LIB_TARGET_STUBV_STUBVISELLOWERING_H

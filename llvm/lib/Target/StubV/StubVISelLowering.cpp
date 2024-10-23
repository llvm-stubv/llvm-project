//===-- StubVISelLowering.cpp - Stub-V DAG Lowering Implementation  -------===//
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

#include "StubVISelLowering.h"
#include "StubV.h"
#include "StubVRegisterInfo.h"
#include "StubVSubtarget.h"
#include "StubVTargetMachine.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"

using namespace llvm;

#define DEBUG_TYPE "stubv-lower"

StubVTargetLowering::StubVTargetLowering(const TargetMachine &TM,
                                         const StubVSubtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {
  MVT XLenVT = Subtarget.getXLenVT();

  // Set up the register classes.
  addRegisterClass(XLenVT, &StubV::GPRRegClass);

  // Compute derived properties from the register classes.
  computeRegisterProperties(STI.getRegisterInfo());


  setOperationAction(ISD::CTPOP, MVT::i8, Custom);
  setOperationAction(ISD::CTPOP, MVT::i32, Custom);
}

void StubVTargetLowering::analyzeInputArgs(
    MachineFunction &MF, CCState &CCInfo,
    const SmallVectorImpl<ISD::InputArg> &Ins, bool IsRet) const {
  unsigned NumArgs = Ins.size();
  FunctionType *FType = MF.getFunction().getFunctionType();

  for (unsigned i = 0; i != NumArgs; ++i) {
    MVT ArgVT = Ins[i].VT;
    ISD::ArgFlagsTy ArgFlags = Ins[i].Flags;

    Type *ArgTy = nullptr;
    if (IsRet)
      ArgTy = FType->getReturnType();
    else if (Ins[i].isOrigArg())
      ArgTy = FType->getParamType(Ins[i].getOrigArgIndex());

    // We support only one ABI (for now)
    StubVABI::ABI ABI = StubVABI::ABI_ILP32;

    if (StubV::CC_StubV(MF.getDataLayout(), ABI, i, ArgVT, ArgVT,
                        CCValAssign::Full, ArgFlags, CCInfo,
                        /*IsFixed=*/true, IsRet, ArgTy, *this)) {
      LLVM_DEBUG(dbgs() << "InputArg #" << i << " has unhandled type " << ArgVT
                        << '\n');
      llvm_unreachable(nullptr);
    }
  }
}

void StubVTargetLowering::analyzeOutputArgs(
    MachineFunction &MF, CCState &CCInfo,
    const SmallVectorImpl<ISD::OutputArg> &Outs, bool IsRet,
    CallLoweringInfo *CLI) const {
  unsigned NumArgs = Outs.size();

  SmallVector<Type *, 4> TypeList;
  if (IsRet)
    TypeList.push_back(MF.getFunction().getReturnType());
  else if (CLI)
    for (const TargetLowering::ArgListEntry &Arg : CLI->getArgs())
      TypeList.push_back(Arg.Ty);

  for (unsigned i = 0; i != NumArgs; i++) {
    MVT ArgVT = Outs[i].VT;
    ISD::ArgFlagsTy ArgFlags = Outs[i].Flags;
    Type *OrigTy = CLI ? CLI->getArgs()[Outs[i].OrigArgIndex].Ty : nullptr;

    // We support only one ABI (for now)
    StubVABI::ABI ABI = StubVABI::ABI_ILP32;

    if (StubV::CC_StubV(MF.getDataLayout(), ABI, i, ArgVT, ArgVT,
                        CCValAssign::Full, ArgFlags, CCInfo, Outs[i].IsFixed,
                        IsRet, OrigTy, *this)) {
      LLVM_DEBUG(dbgs() << "OutputArg #" << i << " has unhandled type " << ArgVT
                        << "\n");
      llvm_unreachable(nullptr);
    }
  }
}

// The caller is responsible for loading the full value if the argument is
// passed with CCValAssign::Indirect.
static SDValue unpackFromRegLoc(SelectionDAG &DAG, SDValue Chain,
                                const CCValAssign &VA, const SDLoc &DL,
                                const ISD::InputArg &In,
                                const StubVTargetLowering &TLI) {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();
  EVT LocVT = VA.getLocVT();
  SDValue Val;
  const TargetRegisterClass *RC = TLI.getRegClassFor(LocVT.getSimpleVT());
  Register VReg = RegInfo.createVirtualRegister(RC);
  RegInfo.addLiveIn(VA.getLocReg(), VReg);
  Val = DAG.getCopyFromReg(Chain, DL, VReg, LocVT);

  if (VA.getLocInfo() == CCValAssign::Indirect)
    return Val;

  assert(VA.getLocInfo() == CCValAssign::Full &&
         "Only support full assignment");
  return Val;
}

// The caller is responsible for loading the full value if the argument is
// passed with CCValAssign::Indirect.
static SDValue unpackFromMemLoc(SelectionDAG &DAG, SDValue Chain,
                                const CCValAssign &VA, const SDLoc &DL) {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  EVT LocVT = VA.getLocVT();
  EVT ValVT = VA.getValVT();
  EVT PtrVT = MVT::getIntegerVT(DAG.getDataLayout().getPointerSizeInBits(0));
  int FI = MFI.CreateFixedObject(ValVT.getStoreSize(), VA.getLocMemOffset(),
                                 /*IsImmutable=*/true);
  SDValue FIN = DAG.getFrameIndex(FI, PtrVT);
  SDValue Val;

  ISD::LoadExtType ExtType;
  switch (VA.getLocInfo()) {
  default:
    llvm_unreachable("Unexpected CCValAssign::LocInfo");
  case CCValAssign::Full:
  case CCValAssign::Indirect:
  case CCValAssign::BCvt:
    ExtType = ISD::NON_EXTLOAD;
    break;
  }
  Val = DAG.getExtLoad(
      ExtType, DL, LocVT, Chain, FIN,
      MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), FI), ValVT);
  return Val;
}

// Transform physical registers into virtual registers.
SDValue StubVTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  if (IsVarArg)
    report_fatal_error("varargs unsupported");
  if (CallConv != CallingConv::C)
    report_fatal_error("Unsupported calling convention");

  MachineFunction &MF = DAG.getMachineFunction();

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());

  analyzeInputArgs(MF, CCInfo, Ins, /*IsRet=*/false);

  for (unsigned i = 0, e = ArgLocs.size(), InsIdx = 0; i != e; ++i, ++InsIdx) {
    CCValAssign &VA = ArgLocs[i];
    SDValue ArgValue;
    if (VA.isRegLoc())
      ArgValue = unpackFromRegLoc(DAG, Chain, VA, DL, Ins[InsIdx], *this);
    else
      ArgValue = unpackFromMemLoc(DAG, Chain, VA, DL);

    assert(VA.getLocInfo() != CCValAssign::Indirect &&
           "Cannot handle Indirect argument assignments yet");
    InVals.push_back(ArgValue);
  }

  return Chain;
}

SDValue
StubVTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                 bool IsVarArg,
                                 const SmallVectorImpl<ISD::OutputArg> &Outs,
                                 const SmallVectorImpl<SDValue> &OutVals,
                                 const SDLoc &DL, SelectionDAG &DAG) const {
  if (IsVarArg)
    report_fatal_error("varargs unsupported");
  if (CallConv != CallingConv::C)
    report_fatal_error("Unsupported calling convention");

  // Stores the assignment of the return value to a location.
  SmallVector<CCValAssign, 16> RVLocs;

  // Info about the registers and stack slot.
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  analyzeOutputArgs(DAG.getMachineFunction(), CCInfo, Outs, /*IsRet=*/true,
                    /*CallLoweringInfo=*/nullptr);

  SDValue Glue;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0, e = RVLocs.size(), OutIdx = 0; i < e; ++i, ++OutIdx) {
    SDValue Val = OutVals[OutIdx];
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    assert(VA.getLocVT() == MVT::i32 && "Only 32-bit for now");
    // Handle a 'normal' return.
    assert(VA.getLocInfo() == CCValAssign::Full &&
           "Only support full assignment");
    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Val, Glue);

    // Guarantee that all emitted copies are stuck together.
    Glue = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain; // Update chain.

  // Add the glue node if we have it.
  if (Glue.getNode()) {
    RetOps.push_back(Glue);
  }

  return DAG.getNode(StubVISD::RET_GLUE, DL, MVT::Other, RetOps);
}

// Implements the StubV calling convention. Returns true upon failure.
bool StubV::CC_StubV(const DataLayout &DL, StubVABI::ABI ABI, unsigned ValNo,
                     MVT ValVT, MVT LocVT, CCValAssign::LocInfo LocInfo,
                     ISD::ArgFlagsTy ArgFlags, CCState &State, bool IsFixed,
                     bool IsRet, Type *OrigTy, const StubVTargetLowering &TLI) {
  unsigned XLen = DL.getLargestLegalIntTypeSizeInBits();
  // Only 32-bit for now
  assert(XLen == 32);
  MVT XLenVT = XLen == 32 ? MVT::i32 : MVT::i64;

  assert(!ArgFlags.isNest() && "Cannot handle nest arguments yet");

  // Any return value split in to more than two values can't be returned
  // directly. Vectors are returned via the available vector registers.
  if (!LocVT.isVector() && IsRet && ValNo > 1)
    return true;

  // A lot of complications arise from the various RISC-V floating-point ABIs:
  // we don't handle those just yet.
  assert(ValVT.isScalarInteger() && "Only supporting integers for now");

  ArrayRef<MCPhysReg> ArgGPRs = StubV::getArgGPRs(ABI);

  SmallVectorImpl<CCValAssign> &PendingLocs = State.getPendingLocs();
  SmallVectorImpl<ISD::ArgFlagsTy> &PendingArgFlags =
      State.getPendingArgFlags();

  assert(PendingLocs.size() == PendingArgFlags.size() &&
         "PendingLocs and PendingArgFlags out of sync");

  assert(!ArgFlags.isSplit() && !ArgFlags.isSplitEnd() &&
         "Cannot handle split arguments yet");

  // Allocate to a register if possible, or else a stack slot.
  unsigned StoreSizeBytes = XLen / 8;
  Align StackAlign = Align(XLen / 8);

  Register Reg = State.AllocateReg(ArgGPRs);

  unsigned StackOffset =
      Reg ? 0 : State.AllocateStack(StoreSizeBytes, StackAlign);

  assert(PendingLocs.empty() && "Cannot handle PendingLocs");

  assert(LocVT == XLenVT && "Expected an XLenVT type at this stage");

  if (Reg) {
    State.addLoc(CCValAssign::getReg(ValNo, ValVT, Reg, LocVT, LocInfo));
    return false;
  }

  State.addLoc(CCValAssign::getMem(ValNo, ValVT, StackOffset, LocVT, LocInfo));
  return false;
}

ArrayRef<MCPhysReg> StubV::getArgGPRs(const StubVABI::ABI ABI) {
  // The GPRs used for passing arguments in the ILP32* and LP64* ABIs, except
  // the ILP32E ABI.
  static const MCPhysReg ArgIGPRs[] = {StubV::X10, StubV::X11, StubV::X12,
                                       StubV::X13, StubV::X14, StubV::X15,
                                       StubV::X16, StubV::X17};

  return ArrayRef(ArgIGPRs);
}

const char *StubVTargetLowering::getTargetNodeName(unsigned Opcode) const {
#define NODE_NAME_CASE(NODE)                                                   \
  case StubVISD::NODE:                                                         \
    return "StubVISD::" #NODE;
  // clang-format off
  switch ((StubVISD::NodeType)Opcode) {
  case StubVISD::FIRST_NUMBER:
    break;
  NODE_NAME_CASE(RET_GLUE)
  NODE_NAME_CASE(CTPOP_MAGIC)
  }
  // clang-format on
  return nullptr;
#undef NODE_NAME_CASE
}

SDValue StubVTargetLowering::LowerOperation(SDValue Op,
                                            SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  default:
    report_fatal_error("unimplemented operand");
  case ISD::CTPOP:
    return lowerCTPOP(Op, DAG);
  }
}

SDValue StubVTargetLowering::lowerCTPOP(SDValue Op, SelectionDAG &DAG) const {
  SDLoc DL(Op);
  SDValue Op0 = Op.getOperand(0);
  SDValue Res = DAG.getNode(StubVISD::CTPOP_MAGIC, DL, Op.getValueType(), Op0,
                            DAG.getConstant(0, DL, Op0.getValueType()));
  return Res;
}

void StubVTargetLowering::ReplaceNodeResults(SDNode *N,
                                             SmallVectorImpl<SDValue> &Results,
                                             SelectionDAG &DAG) const {
  SDLoc DL(N);
  switch (N->getOpcode()) {
  default:
    llvm_unreachable("Don't know how to custom type legalize this operation!");
  case ISD::CTPOP:
    auto ExtOp = DAG.getNode(ISD::ZERO_EXTEND, DL, MVT::i32, N->getOperand(0));
    auto NewCTPOP = DAG.getNode(StubVISD::CTPOP_MAGIC, DL, MVT::i32, ExtOp,
                                DAG.getConstant(0, DL, MVT::i32));
    auto Trunc = DAG.getNode(ISD::TRUNCATE, DL, MVT::i8, NewCTPOP);
    Results.push_back(Trunc);
    return;
  }
}

SDValue StubVTargetLowering::PerformDAGCombine(SDNode *N,
                                               DAGCombinerInfo &DCI) const {
  SelectionDAG &DAG = DCI.DAG;
  SDLoc DL(N);

  switch (N->getOpcode()) {
  default:
    break;
  case StubVISD::CTPOP_MAGIC: {
    auto Op0 = N->getOperand(0);

    if (Op0.getOpcode() != ISD::ADD || !isa<ConstantSDNode>(Op0.getOperand(1)))
      break;

    return DAG.getNode(StubVISD::CTPOP_MAGIC, DL, MVT::i32, Op0.getOperand(0),
                       Op0.getOperand(1));
  }
  }

  return SDValue();
}

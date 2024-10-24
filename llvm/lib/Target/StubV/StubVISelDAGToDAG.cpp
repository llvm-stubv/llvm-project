//===-- StubVISelDAGToDAG.cpp - A dag to dag inst selector for Stub-V -----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the Stub-V target.
//
//===----------------------------------------------------------------------===//

#include "StubVISelDAGToDAG.h"
#include <memory>

using namespace llvm;

#define DEBUG_TYPE "stubv-isel"
#define PASS_NAME "Stub-V DAG->DAG Pattern Instruction Selection"

void StubVDAGToDAGISel::Select(SDNode *Node) {
  // If we have a custom node, we have already selected.
  if (Node->isMachineOpcode()) {
    LLVM_DEBUG(dbgs() << "== "; Node->dump(CurDAG); dbgs() << "\n");
    Node->setNodeId(-1);
    return;
  }

  // Select the default instruction.
  SelectCode(Node);
}

// This pass converts a legalized DAG into a StubV-specific DAG, ready
// for instruction scheduling.
FunctionPass *llvm::createStubVISelDag(StubVTargetMachine &TM,
                                       CodeGenOptLevel OptLevel) {
  return new StubVDAGToDAGISelLegacy(TM, OptLevel);
}

char StubVDAGToDAGISelLegacy::ID = 0;

StubVDAGToDAGISelLegacy::StubVDAGToDAGISelLegacy(StubVTargetMachine &TM,
                                                 CodeGenOptLevel OptLevel)
    : SelectionDAGISelLegacy(
          ID, std::make_unique<StubVDAGToDAGISel>(TM, OptLevel)) {}

INITIALIZE_PASS(StubVDAGToDAGISelLegacy, DEBUG_TYPE, PASS_NAME, false, false)

bool StubVDAGToDAGISel::SelectAddrRegImm(SDValue Addr, SDValue &Base,
                                         SDValue &Offset) {
  SDLoc DL(Addr);
  MVT VT = Addr.getSimpleValueType();

  if (CurDAG->isBaseWithConstantOffset(Addr)) {
    int64_t CVal = cast<ConstantSDNode>(Addr.getOperand(1))->getSExtValue();
    if (isInt<12>(CVal)) {
      Base = Addr.getOperand(0);

      if (auto *FIN = dyn_cast<FrameIndexSDNode>(Base))
        Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), VT);
      Offset = CurDAG->getTargetConstant(CVal, DL, VT);
      return true;
    }
  }

  // Handle ADD with large immediates.
  if (Addr.getOpcode() == ISD::ADD && isa<ConstantSDNode>(Addr.getOperand(1))) {
    int64_t CVal = cast<ConstantSDNode>(Addr.getOperand(1))->getSExtValue();
    assert(!isInt<12>(CVal) && "simm12 not already handled?");

    // Handle immediates in the range [-4096,-2049] or [2048, 4094]. We can use
    // an ADDI for part of the offset and fold the rest into the load/store.
    if (isInt<12>(CVal / 2) && isInt<12>(CVal - CVal / 2)) {
      int64_t Adj = CVal < 0 ? -2048 : 2047;
      Base = SDValue(
          CurDAG->getMachineNode(StubV::ADDI, DL, VT, Addr.getOperand(0),
                                 CurDAG->getTargetConstant(Adj, DL, VT)),
          0);
      Offset = CurDAG->getTargetConstant(CVal - Adj, DL, VT);
      return true;
    }

  }

  Base = Addr;
  Offset = CurDAG->getTargetConstant(0, DL, VT);
  return true;
}

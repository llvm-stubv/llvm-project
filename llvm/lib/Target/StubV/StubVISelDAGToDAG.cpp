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

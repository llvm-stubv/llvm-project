add_llvm_component_group(StubV)

set(LLVM_TARGET_DEFINITIONS StubV.td)

tablegen(LLVM StubVGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM StubVGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM StubVGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM StubVGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM StubVGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM StubVGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM StubVGenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM StubVGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM StubVGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(StubVCommonTableGen)

add_llvm_target(StubVCodeGen
  StubVAsmPrinter.cpp
  StubVFrameLowering.cpp
  StubVISelDAGToDAG.cpp
  StubVISelLowering.cpp
  StubVInstrInfo.cpp
  StubVRegisterInfo.cpp
  StubVSubtarget.cpp
  StubVTargetObjectFile.cpp
  StubVTargetMachine.cpp

  LINK_COMPONENTS
  AsmPrinter
  Core
  MC
  Support
  StubVDesc
  StubVInfo
  Target
  TargetParser

  ADD_TO_COMPONENT
  StubV
  )

add_subdirectory(AsmParser)
add_subdirectory(Disassembler)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)

add_llvm_component_group(StubV)

add_llvm_target(StubVCodeGen
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

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)

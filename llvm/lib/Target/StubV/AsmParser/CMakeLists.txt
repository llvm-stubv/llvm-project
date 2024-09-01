add_llvm_component_library(LLVMStubVAsmParser
  StubVAsmParser.cpp

  LINK_COMPONENTS
  MC
  MCParser
  StubVDesc
  StubVInfo
  Support
  TargetParser

  ADD_TO_COMPONENT
  StubV
  )


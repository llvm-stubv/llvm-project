add_llvm_component_library(LLVMStubVDisassembler
  StubVDisassembler.cpp

  LINK_COMPONENTS
  MC
  MCDisassembler
  StubVDesc
  StubVInfo
  Support

  ADD_TO_COMPONENT
  StubV
  )

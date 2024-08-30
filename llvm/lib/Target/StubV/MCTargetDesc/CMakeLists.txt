add_llvm_component_library(LLVMStubVDesc
  StubVMCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  StubVInfo
  Support
  TargetParser

  ADD_TO_COMPONENT
  StubV
)


add_llvm_component_library(LLVMStubVInfo
  StubVTargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  StubV
  )

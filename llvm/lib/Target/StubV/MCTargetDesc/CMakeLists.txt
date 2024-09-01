add_llvm_component_library(LLVMStubVDesc
  StubVAsmBackend.cpp
  StubVELFObjectWriter.cpp
  StubVInstPrinter.cpp
  StubVMCAsmInfo.cpp
  StubVMCCodeEmitter.cpp
  StubVMCObjectFileInfo.cpp
  StubVMCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  StubVInfo
  Support
  TargetParser

  ADD_TO_COMPONENT
  StubV
)


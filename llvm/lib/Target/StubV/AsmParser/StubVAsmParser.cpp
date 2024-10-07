//===-- StubVAsmParser.cpp - Parse StubV assembly to MCInst instructions -===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/StubVBaseInfo.h"
#include "MCTargetDesc/StubVInstPrinter.h"
#include "TargetInfo/StubVTargetInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Casting.h"

using namespace llvm;

#define DEBUG_TYPE "stubv-asm-parser"

namespace {
struct StubVOperand;

class StubVAsmParser : public MCTargetAsmParser {

  SMLoc getLoc() const { return getParser().getTok().getLoc(); }

  bool parseRegister(MCRegister &Reg, SMLoc &StartLoc, SMLoc &EndLoc) override;
  ParseStatus tryParseRegister(MCRegister &Reg, SMLoc &StartLoc,
                               SMLoc &EndLoc) override;

  bool parseOperand(OperandVector &Operands, StringRef Mnemonic);

  bool parseDirectiveAttribute();

  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;

  ParseStatus parseDirective(AsmToken DirectiveID) override;

  bool generateImmOutOfRangeError(OperandVector &Operands, uint64_t ErrorInfo,
                                  int64_t Lower, int64_t Upper,
                                  const Twine &Msg);
  bool generateImmOutOfRangeError(SMLoc ErrorLoc, int64_t Lower, int64_t Upper,
                                  const Twine &Msg);

  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;

  MCRegister matchRegisterNameHelper(StringRef Name) const;

  /// Helper for processing MC instructions that have been successfully matched
  /// by MatchAndEmitInstruction. Modifications to the emitted instructions,
  /// like the expansion of pseudo instructions (e.g., "li"), can be performed
  /// in this method.
  bool processInstruction(MCInst &Inst, SMLoc IDLoc, OperandVector &Operands,
                          MCStreamer &Out);

// Auto-generated instruction matching functions
#define GET_ASSEMBLER_HEADER
#include "StubVGenAsmMatcher.inc"

  ParseStatus parseImmediate(OperandVector &Operands);
  ParseStatus parseMemOpBaseReg(OperandVector &Operands);
  ParseStatus parseRegister(OperandVector &Operands, bool AllowParens = false);

public:
  enum StubVMatchResultTy {
    Match_Dummy = FIRST_TARGET_MATCH_RESULT_TY,
    Match_RequiresEvenGPRs,
#define GET_OPERAND_DIAGNOSTIC_TYPES
#include "StubVGenAsmMatcher.inc"
#undef GET_OPERAND_DIAGNOSTIC_TYPES
  };

  StubVAsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
                 const MCInstrInfo &MII, const MCTargetOptions &Options)
      : MCTargetAsmParser(Options, STI, MII) {
    MCAsmParserExtension::Initialize(Parser);
    setAvailableFeatures(ComputeAvailableFeatures(STI.getFeatureBits()));
  }
};

/// StubVOperand - Instances of this class represent a parsed machine
/// instruction
struct StubVOperand final : public MCParsedAsmOperand {

  enum class KindTy {
    Token,
    Register,
    Immediate,
  } Kind;

  struct RegOp {
    MCRegister RegNum;
  };

  struct ImmOp {
    const MCExpr *Val;
  };

  SMLoc StartLoc, EndLoc;
  union {
    StringRef Tok;
    RegOp Reg;
    ImmOp Imm;
  };

  StubVOperand(KindTy K) : Kind(K) {}

public:
  StubVOperand(const StubVOperand &o) : MCParsedAsmOperand() {
    Kind = o.Kind;
    StartLoc = o.StartLoc;
    EndLoc = o.EndLoc;
    switch (Kind) {
    case KindTy::Register:
      Reg = o.Reg;
      break;
    case KindTy::Immediate:
      Imm = o.Imm;
      break;
    case KindTy::Token:
      Tok = o.Tok;
      break;
    }
  }

  bool isToken() const override { return Kind == KindTy::Token; }
  bool isReg() const override { return Kind == KindTy::Register; }
  bool isImm() const override { return Kind == KindTy::Immediate; }
  bool isMem() const override { return false; }

  bool isSImm12() const {
    if (!isImm())
      return false;
    int64_t Imm;
    if (!evaluateConstantImm(getImm(), Imm))
      return false;
    return isInt<12>(fixImmediateForRV32(Imm));
  }

  static bool evaluateConstantImm(const MCExpr *Expr, int64_t &Imm) {
    if (const auto *CE = dyn_cast<MCConstantExpr>(Expr)) {
      Imm = CE->getValue();
      return true;
    }

    return false;
  }

  // If this a RV32 and the immediate is a uimm32, sign extend it to 32 bits.
  // This allows writing 'addi a0, a0, 0xffffffff'.
  static int64_t fixImmediateForRV32(int64_t Imm) {
    if (!isUInt<32>(Imm))
      return Imm;
    return SignExtend64<32>(Imm);
  }

  /// getStartLoc - Gets location of the first token of this operand
  SMLoc getStartLoc() const override { return StartLoc; }
  /// getEndLoc - Gets location of the last token of this operand
  SMLoc getEndLoc() const override { return EndLoc; }

  MCRegister getReg() const override {
    assert(Kind == KindTy::Register && "Invalid type access!");
    return Reg.RegNum;
  }

  const MCExpr *getImm() const {
    assert(Kind == KindTy::Immediate && "Invalid type access!");
    return Imm.Val;
  }

  StringRef getToken() const {
    assert(Kind == KindTy::Token && "Invalid type access!");
    return Tok;
  }

  void print(raw_ostream &OS) const override {
    auto RegName = [](MCRegister Reg) {
      if (Reg)
        return StubVInstPrinter::getRegisterName(Reg);
      return "noreg";
    };

    switch (Kind) {
    case KindTy::Immediate:
      OS << *getImm();
      break;
    case KindTy::Register:
      OS << "<register " << RegName(getReg()) << ">";
      break;
    case KindTy::Token:
      OS << "'" << getToken() << "'";
      break;
    }
  }

  static std::unique_ptr<StubVOperand> createToken(StringRef Str, SMLoc S) {
    auto Op = std::make_unique<StubVOperand>(KindTy::Token);
    Op->Tok = Str;
    Op->StartLoc = S;
    Op->EndLoc = S;
    return Op;
  }

  static std::unique_ptr<StubVOperand>
  createReg(unsigned RegNo, SMLoc S, SMLoc E) {
    auto Op = std::make_unique<StubVOperand>(KindTy::Register);
    Op->Reg.RegNum = RegNo;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<StubVOperand> createImm(const MCExpr *Val, SMLoc S,
                                                 SMLoc E) {
    auto Op = std::make_unique<StubVOperand>(KindTy::Immediate);
    Op->Imm.Val = Val;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  // Used by the TableGen Code
  void addRegOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createReg(getReg()));
  }

  static void addExpr(MCInst &Inst, const MCExpr *Expr) {
    assert(Expr && "Expr shouldn't be null!");
    int64_t Imm = 0;
    bool IsConstant = evaluateConstantImm(Expr, Imm);
    if (IsConstant)
      Inst.addOperand(MCOperand::createImm(fixImmediateForRV32(Imm)));
    else
      Inst.addOperand(MCOperand::createExpr(Expr));
  }

  void addImmOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    addExpr(Inst, getImm());
  }
};
} // namespace

#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "StubVGenAsmMatcher.inc"

bool StubVAsmParser::processInstruction(MCInst &Inst, SMLoc IDLoc,
                                        OperandVector &Operands,
                                        MCStreamer &Out) {
  Inst.setLoc(IDLoc);
  return true;
}

bool StubVAsmParser::generateImmOutOfRangeError(
    SMLoc ErrorLoc, int64_t Lower, int64_t Upper,
    const Twine &Msg = "immediate must be an integer in the range") {
  return Error(ErrorLoc, Msg + " [" + Twine(Lower) + ", " + Twine(Upper) + "]");
}

bool StubVAsmParser::generateImmOutOfRangeError(
    OperandVector &Operands, uint64_t ErrorInfo, int64_t Lower, int64_t Upper,
    const Twine &Msg = "immediate must be an integer in the range") {
  SMLoc ErrorLoc = ((StubVOperand &)*Operands[ErrorInfo]).getStartLoc();
  return generateImmOutOfRangeError(ErrorLoc, Lower, Upper, Msg);
}

bool StubVAsmParser::MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                             OperandVector &Operands,
                                             MCStreamer &Out,
                                             uint64_t &ErrorInfo,
                                             bool MatchingInlineAsm) {
  MCInst Inst;
  auto Result =
      MatchInstructionImpl(Operands, Inst, ErrorInfo, MatchingInlineAsm);

  switch (Result) {
  default:
    break;
  case Match_Success:
    processInstruction(Inst, IDLoc, Operands, Out);
    Inst.setLoc(IDLoc);
    Out.emitInstruction(Inst, getSTI());
    return false;
  case Match_MissingFeature:
    return Error(IDLoc, "instruction use requires an option to be enabled");
  case Match_MnemonicFail:
    return Error(IDLoc, "unrecognized instruction mnemonic");
  case Match_InvalidOperand: {
    SMLoc ErrorLoc = IDLoc;
    if (ErrorInfo != ~0U) {
      if (ErrorInfo >= Operands.size())
        return Error(ErrorLoc, "too few operands for instruction");

      ErrorLoc = ((StubVOperand &)*Operands[ErrorInfo]).getStartLoc();
      if (ErrorLoc == SMLoc())
        ErrorLoc = IDLoc;
    }
    return Error(ErrorLoc, "invalid operand for instruction");
  }
  case Match_InvalidSImm12:
    return generateImmOutOfRangeError(Operands, ErrorInfo, -(1 << 11),
                                      (1 << 11) - 1);
  }
  report_fatal_error("Unknown match type detected!");
}

bool StubVAsmParser::parseRegister(MCRegister &Reg, SMLoc &StartLoc,
                                   SMLoc &EndLoc) {
  if (!tryParseRegister(Reg, StartLoc, EndLoc).isSuccess())
    return Error(StartLoc, "invalid register name");
  return false;
}

// Attempts to match Name as a register (either using the default name or
// alternative ABI names), setting RegNo to the matching register. Upon
// failure, returns a non-valid MCRegister. If IsRVE, then registers x16-x31
// will be rejected.
MCRegister StubVAsmParser::matchRegisterNameHelper(StringRef Name) const {
  MCRegister Reg = MatchRegisterName(Name);
  if (!Reg)
    Reg = MatchRegisterAltName(Name);
  return Reg;
}

ParseStatus StubVAsmParser::tryParseRegister(MCRegister &Reg, SMLoc &StartLoc,
                                             SMLoc &EndLoc) {
  const AsmToken &Tok = getParser().getTok();
  StartLoc = Tok.getLoc();
  EndLoc = Tok.getEndLoc();
  StringRef Name = getLexer().getTok().getIdentifier();

  Reg = matchRegisterNameHelper(Name);
  if (!Reg)
    return ParseStatus::NoMatch;

  getParser().Lex(); // Eat identifier token.
  return ParseStatus::Success;
}

bool StubVAsmParser::parseOperand(OperandVector &Operands, StringRef Mnemonic) {
  // Attempt to parse token as a register.
  if (parseRegister(Operands, true).isSuccess())
    return false;

  // Attempt to parse token as an immediate
  if (parseImmediate(Operands).isSuccess()) {
    // Parse memory base register if present
    if (getLexer().is(AsmToken::LParen))
      return !parseMemOpBaseReg(Operands).isSuccess();
    return false;
  }

  // Finally we have exhausted all options and must declare defeat.
  Error(getLoc(), "unknown operand");
  return true;
}

bool StubVAsmParser::ParseInstruction(ParseInstructionInfo &Info,
                                      StringRef Name, SMLoc NameLoc,
                                      OperandVector &Operands) {
  // First operand is token for instruction
  Operands.push_back(StubVOperand::createToken(Name, NameLoc));

  // If there are no more operands, then finish
  if (getLexer().is(AsmToken::EndOfStatement)) {
    getParser().Lex(); // Consume the EndOfStatement.
    return false;
  }

  // Parse first operand
  if (parseOperand(Operands, Name))
    return true;

  // Parse until end of statement, consuming commas between operands
  while (parseOptionalToken(AsmToken::Comma)) {
    // Parse next operand
    if (parseOperand(Operands, Name))
      return true;
  }

  if (getParser().parseEOL("unexpected token")) {
    getParser().eatToEndOfStatement();
    return true;
  }
  return false;
}

ParseStatus StubVAsmParser::parseDirective(AsmToken DirectiveID) {
  StringRef IDVal = DirectiveID.getString();

  if (IDVal == ".attribute")
    return parseDirectiveAttribute();

  return ParseStatus::NoMatch;
}

/// parseDirectiveAttribute
///  ::= .attribute expression ',' ( expression | "string" )
///  ::= .attribute identifier ',' ( expression | "string" )
bool StubVAsmParser::parseDirectiveAttribute() {
  MCAsmParser &Parser = getParser();
  int64_t Tag;
  SMLoc TagLoc;
  TagLoc = Parser.getTok().getLoc();
  if (Parser.getTok().is(AsmToken::Identifier)) {
    // Just consume but throw away, for compatibility
    Parser.Lex();
  } else {
    const MCExpr *AttrExpr;

    TagLoc = Parser.getTok().getLoc();
    if (Parser.parseExpression(AttrExpr))
      return true;

    const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(AttrExpr);
    if (check(!CE, TagLoc, "expected numeric constant"))
      return true;

    Tag = CE->getValue();
  }

  if (Parser.parseComma())
    return true;

  StringRef StringValue;
  bool IsIntegerValue = true;

  // RISC-V attributes have a string value if the tag number is odd
  // and an integer value if the tag number is even.
  if (Tag % 2)
    IsIntegerValue = false;

  SMLoc ValueExprLoc = Parser.getTok().getLoc();
  if (IsIntegerValue) {
    const MCExpr *ValueExpr;
    if (Parser.parseExpression(ValueExpr))
      return true;

    const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(ValueExpr);
    if (!CE)
      return Error(ValueExprLoc, "expected numeric constant");
  } else {
    if (Parser.getTok().isNot(AsmToken::String))
      return Error(Parser.getTok().getLoc(), "expected string constant");

    StringValue = Parser.getTok().getStringContents();
    Parser.Lex();
  }

  if (Parser.parseEOL())
    return true;

  return false;
}

ParseStatus StubVAsmParser::parseRegister(OperandVector &Operands,
                                          bool AllowParens) {
  SMLoc FirstS = getLoc();
  bool HadParens = false;
  AsmToken LParen;

  // If this is an LParen and a parenthesised register name is allowed, parse it
  // atomically.
  if (AllowParens && getLexer().is(AsmToken::LParen)) {
    AsmToken Buf[2];
    size_t ReadCount = getLexer().peekTokens(Buf);
    if (ReadCount == 2 && Buf[1].getKind() == AsmToken::RParen) {
      HadParens = true;
      LParen = getParser().getTok();
      getParser().Lex(); // Eat '('
    }
  }

  switch (getLexer().getKind()) {
  default:
    if (HadParens)
      getLexer().UnLex(LParen);
    return ParseStatus::NoMatch;
  case AsmToken::Identifier:
    StringRef Name = getLexer().getTok().getIdentifier();
    MCRegister RegNo = matchRegisterNameHelper(Name);

    if (!RegNo) {
      if (HadParens)
        getLexer().UnLex(LParen);
      return ParseStatus::NoMatch;
    }
    if (HadParens)
      Operands.push_back(StubVOperand::createToken("(", FirstS));
    SMLoc S = getLoc();
    SMLoc E = SMLoc::getFromPointer(S.getPointer() + Name.size());
    getLexer().Lex();
    Operands.push_back(StubVOperand::createReg(RegNo, S, E));
  }

  if (HadParens) {
    getParser().Lex(); // Eat ')'
    Operands.push_back(StubVOperand::createToken(")", getLoc()));
  }

  return ParseStatus::Success;
}

ParseStatus StubVAsmParser::parseImmediate(OperandVector &Operands) {
  SMLoc S = getLoc();
  SMLoc E;
  const MCExpr *Res;

  switch (getLexer().getKind()) {
  default:
    return ParseStatus::NoMatch;
  case AsmToken::LParen:
  case AsmToken::Dot:
  case AsmToken::Minus:
  case AsmToken::Plus:
  case AsmToken::Exclaim:
  case AsmToken::Tilde:
  case AsmToken::Integer:
  case AsmToken::String:
  case AsmToken::Identifier:
    if (getParser().parseExpression(Res, E))
      return ParseStatus::Failure;
    break;
  }

  Operands.push_back(StubVOperand::createImm(Res, S, E));
  return ParseStatus::Success;
}

ParseStatus StubVAsmParser::parseMemOpBaseReg(OperandVector &Operands) {
  if (parseToken(AsmToken::LParen, "expected '('"))
    return ParseStatus::Failure;
  Operands.push_back(StubVOperand::createToken("(", getLoc()));

  if (!parseRegister(Operands).isSuccess())
    return Error(getLoc(), "expected register");

  if (parseToken(AsmToken::RParen, "expected ')'"))
    return ParseStatus::Failure;
  Operands.push_back(StubVOperand::createToken(")", getLoc()));

  return ParseStatus::Success;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeStubVAsmParser() {
  RegisterMCAsmParser<StubVAsmParser> X(getTheStubV32Target());
}

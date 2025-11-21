#include "TestNamingCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/MacroArgs.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"

using namespace clang::ast_matchers;

namespace clang::tidy::yt {

namespace {

class TestNamingPPCallbacks : public PPCallbacks {
public:
  TestNamingPPCallbacks(TestNamingCheck &Check, Preprocessor &PP)
      : Check(Check), PP(PP) {}

  void MacroExpands(const Token &MacroNameTok, const MacroDefinition &MD,
                    SourceRange Range, const MacroArgs *Args) override {
    if (!Args || Args->getNumMacroArguments() < 2)
      return;

    StringRef MacroName = MacroNameTok.getIdentifierInfo()->getName();
    if (MacroName != "TEST_F" && MacroName != "TEST_P")
      return;

    // Get the test fixture name (first argument)
    const Token *FixtureTokens = Args->getUnexpArgument(0);
    if (!FixtureTokens || !FixtureTokens->is(tok::identifier))
      return;
    StringRef FixtureName = FixtureTokens->getIdentifierInfo()->getName();

    // Get the test name (second argument)
    const Token *TestTokens = Args->getUnexpArgument(1);
    if (!TestTokens || !TestTokens->is(tok::identifier))
      return;
    StringRef TestName = TestTokens->getIdentifierInfo()->getName();

    SourceLocation MacroLoc = MacroNameTok.getLocation();

    // Check if test fixture class name ends with "Test"
    if (!FixtureName.ends_with("Test")) {
      Check.diag(MacroLoc,
                 "test fixture class name '%0' should end with 'Test'")
          << FixtureName;
    }

    // Check if test name starts with "Test"
    if (TestName.starts_with("Test")) {
      StringRef SuggestedName = TestName.substr(4); // Remove "Test" prefix
      Check.diag(MacroLoc,
                 "test method name '%0' should not start with 'Test' prefix; "
                 "use '%1' instead")
          << TestName << SuggestedName;
    }
  }

private:
  TestNamingCheck &Check;
  Preprocessor &PP;
};

} // namespace

void TestNamingCheck::registerPPCallbacks(const SourceManager &SM,
                                          Preprocessor *PP,
                                          Preprocessor *ModuleExpanderPP) {
  PP->addPPCallbacks(
      std::make_unique<TestNamingPPCallbacks>(*this, *PP));
}

void TestNamingCheck::registerMatchers(MatchFinder *Finder) {
  // We handle everything in the preprocessor callback
  // No AST matchers needed for this check
}

void TestNamingCheck::check(const MatchFinder::MatchResult &Result) {
  // All checking is done in the preprocessor callback
}

} // namespace clang::tidy::yt
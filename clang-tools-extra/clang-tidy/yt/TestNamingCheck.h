#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_TESTNAMING_CHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_TESTNAMING_CHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::yt {

/// Checks that test names follow YTsaurus naming conventions.
///
/// Test fixture class names should end with "Test" (e.g., TPartitionKeysBuilderTest).
/// Test method names should NOT include "Test" prefix (e.g., TwoPartitions, not TestTwoPartitions).
class TestNamingCheck : public ClangTidyCheck {
public:
  TestNamingCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerPPCallbacks(const SourceManager &SM, Preprocessor *PP,
                           Preprocessor *ModuleExpanderPP) override;
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace clang::tidy::yt

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_TESTNAMING_CHECK_H
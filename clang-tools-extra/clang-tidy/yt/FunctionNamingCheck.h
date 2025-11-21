#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_FUNCTIONNAMING_CHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_FUNCTIONNAMING_CHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::yt {

/// Checks that function names follow YTsaurus naming conventions.
///
/// Functions should be in PascalCase (e.g., DoSomething, GetValue).
class FunctionNamingCheck : public ClangTidyCheck {
public:
  FunctionNamingCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace clang::tidy::yt

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_FUNCTIONNAMING_CHECK_H
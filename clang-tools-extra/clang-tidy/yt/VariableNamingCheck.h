#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_VARIABLENAMING_CHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_VARIABLENAMING_CHECK_H

#include "../ClangTidyCheck.h"
#include "NamingUtils.h"

namespace clang::tidy::yt {

class VariableNamingCheck : public ClangTidyCheck {
public:
  VariableNamingCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace clang::tidy::yt

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_VARIABLENAMING_CHECK_H
#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_WAITFORINCONSTRUCTOR_CHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_WAITFORINCONSTRUCTOR_CHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::yt {

/// Checks that WaitFor is not used in constructors.
///
/// Using WaitFor in constructors is surprising for users as it blocks
/// synchronously. Instead, factory functions returning TFuture should be used.
class WaitForInConstructorCheck : public ClangTidyCheck {
public:
  WaitForInConstructorCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace clang::tidy::yt

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_WAITFORINCONSTRUCTOR_CHECK_H
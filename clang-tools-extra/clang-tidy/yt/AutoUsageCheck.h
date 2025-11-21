#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_AUTOUSAGE_CHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_AUTOUSAGE_CHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::yt {

class AutoUsageCheck : public ClangTidyCheck {
public:
  AutoUsageCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  bool isIntOrBoolType(QualType Type) const;
};

} // namespace clang::tidy::yt

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_AUTOUSAGE_CHECK_H
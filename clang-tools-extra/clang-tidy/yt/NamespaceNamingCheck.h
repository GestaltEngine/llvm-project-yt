#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_NAMESPACENAMING_CHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_NAMESPACENAMING_CHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::yt {

class NamespaceNamingCheck : public ClangTidyCheck {
public:
  NamespaceNamingCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  bool isValidNamespaceName(StringRef Name) const;
  std::string toValidNamespaceName(StringRef Name) const;
};

} // namespace clang::tidy::yt

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_NAMESPACENAMING_CHECK_H
#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_GETDOESNOTRETURNNULLPTR_CHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_GETDOESNOTRETURNNULLPTR_CHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::yt {

/// Checks that methods/functions starting with "Get" do not return nullptr.
///
/// Methods that may return nullptr should use "Find" prefix instead of "Get".
/// This check detects functions that:
/// - Start with "Get"
/// - Return a pointer type
/// - May return nullptr (contains return nullptr or return NULL)

class GetDoesNotReturnNullptrCheck : public ClangTidyCheck {
public:
  GetDoesNotReturnNullptrCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace clang::tidy::yt

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_GETDOESNOTRETURNNULLPTR_CHECK_H
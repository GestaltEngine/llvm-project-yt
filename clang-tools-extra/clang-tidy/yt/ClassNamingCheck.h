#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_CLASSNAMINGCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_CLASSNAMINGCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace yt {

/// Checks that class and struct names follow YtSaurus naming conventions:
/// - Classes should start with 'T' and be in PascalCase
/// - Interfaces should start with 'I' and be in PascalCase  
/// - Enums should start with 'E' and be in PascalCase
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/yt-class-naming.html
class ClassNamingCheck : public ClangTidyCheck {
public:
  ClassNamingCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace yt
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_CLASSNAMINGCHECK_H
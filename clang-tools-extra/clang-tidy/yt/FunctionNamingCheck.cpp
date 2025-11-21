#include "FunctionNamingCheck.h"
#include "NamingUtils.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::yt {

void FunctionNamingCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      functionDecl(unless(isImplicit()),
                   unless(cxxConstructorDecl()),
                   unless(cxxDestructorDecl()),
                   unless(cxxConversionDecl())).bind("func"), this);
}

void FunctionNamingCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Func = Result.Nodes.getNodeAs<FunctionDecl>("func");
  if (!Func || Func->isImplicit() || Func->isMain() || Func->isOverloadedOperator())
    return;
  StringRef Name = Func->getName();
  if (Name.starts_with("__") || Name.starts_with("operator"))
    return;
  if (!isPascalCase(Name)) {
    diag(Func->getLocation(),
         "function name '%0' should be in PascalCase")
        << Name;
  }
}

} // namespace clang::tidy::yt
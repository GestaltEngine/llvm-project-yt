//===--- FunctionNamingCheck.cpp - clang-tidy -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "FunctionNamingCheck.h"
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

bool FunctionNamingCheck::isPascalCase(StringRef Name) const {
  if (Name.empty())
    return false;
  if (!std::isupper(Name[0]))
    return false;
  for (char C : Name) {
    if (std::islower(C)) {
      return true;
    }
  }
  return false;
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
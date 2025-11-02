//===--- VariableNamingCheck.cpp - clang-tidy -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "VariableNamingCheck.h"
#include "NamingUtils.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::yt {

void VariableNamingCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(varDecl(unless(isImplicit())).bind("var"), this);
  Finder->addMatcher(parmVarDecl(unless(isImplicit())).bind("param"), this);
}

void VariableNamingCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Var = Result.Nodes.getNodeAs<VarDecl>("var");
  const auto *Param = Result.Nodes.getNodeAs<ParmVarDecl>("param");
  const VarDecl *Decl = Var ? Var : Param;
  if (!Decl || Decl->isImplicit())
    return;
  StringRef Name = Decl->getName();
  if (Name.empty() || Name.starts_with("_"))
    return;

  if (Name == "Logger" || Name == "this_")
    return;

  bool IsLocal = Param || (Var && Var->isLocalVarDecl());
  bool IsGlobal = Var && (Var->hasGlobalStorage() || Var->isStaticDataMember());
  bool IsConstant = Var && Var->getType().isConstQualified() && (IsGlobal || Var->isStaticLocal());

  if (IsLocal && !isCamelCase(Name)) {
    if (IsConstant) {
      if (!isPascalCase(Name)) {
        diag(Decl->getLocation(),
             "local variable or parameter '%0' should be in PascalCase; consider renaming to '%1'")
            << Name << toPascalCase(Name);
      }
    } else {
      if (!isCamelCase(Name)) {
        diag(Decl->getLocation(),
             "local variable or parameter '%0' should be in camelCase; consider renaming to '%1'")
            << Name << toCamelCase(Name);
      }
    }
  } else if ((IsGlobal || IsConstant) && !isPascalCase(Name)) {
    diag(Decl->getLocation(),
         "global/static variable or constant '%0' should be in PascalCase; consider renaming to '%1'")
        << Name << toPascalCase(Name);
  }
}

} // namespace clang::tidy::yt
//===--- VariableNamingCheck.cpp - clang-tidy -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "VariableNamingCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::yt {

void VariableNamingCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(varDecl(unless(isImplicit())).bind("var"), this);
  Finder->addMatcher(parmVarDecl(unless(isImplicit())).bind("param"), this);
}

bool VariableNamingCheck::isCamelCase(StringRef Name) const {
  return !Name.empty() && std::islower(Name[0]);
}

bool VariableNamingCheck::isPascalCase(StringRef Name) const {
  return !Name.empty() && std::isupper(Name[0]);
}

std::string VariableNamingCheck::toCamelCase(StringRef Name) const {
  if (Name.empty())
    return "";
  std::string Result;
  bool capitalizeNext = false, first = true;
  for (char C : Name) {
    if (C == '_') {
      capitalizeNext = true;
      continue;
    }
    if (first) {
      Result += std::tolower(C);
      first = false;
    } else if (capitalizeNext) {
      Result += std::toupper(C);
      capitalizeNext = false;
    } else {
      Result += std::tolower(C);
    }
  }
  return Result;
}

std::string VariableNamingCheck::toPascalCase(StringRef Name) const {
  if (Name.empty())
    return "";
  std::string Result;
  bool capitalizeNext = true;
  for (char C : Name) {
    if (C == '_') {
      capitalizeNext = true;
      continue;
    }
    Result += capitalizeNext ? std::toupper(C) : std::tolower(C);
    capitalizeNext = false;
  }
  return Result;
}

void VariableNamingCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Var = Result.Nodes.getNodeAs<VarDecl>("var");
  const auto *Param = Result.Nodes.getNodeAs<ParmVarDecl>("param");
  const VarDecl *Decl = Var ? Var : Param;
  if (!Decl || Decl->isImplicit())
    return;
  StringRef Name = Decl->getName();
  if (Name.empty() || Name.startswith("_"))
    return;

  bool isLocal = Param || (Var && Var->isLocalVarDecl());
  bool isGlobal = Var && (Var->hasGlobalStorage() || Var->isStaticDataMember());
  bool isConstant = Var && Var->getType().isConstQualified() && (isGlobal || Var->isStaticLocal());

  if (isLocal && !isCamelCase(Name)) {
    diag(Decl->getLocation(),
         "local variable or parameter '%0' should be in camelCase; consider renaming to '%1'")
        << Name << toCamelCase(Name);
  } else if ((isGlobal || isConstant) && !isPascalCase(Name)) {
    diag(Decl->getLocation(),
         "global variable or constant '%0' should be in PascalCase; consider renaming to '%1'")
        << Name << toPascalCase(Name);
  }
}

} // namespace clang::tidy::yt
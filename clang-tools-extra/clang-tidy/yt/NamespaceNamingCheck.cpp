//===--- NamespaceNamingCheck.cpp - clang-tidy ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "NamespaceNamingCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::yt {

void NamespaceNamingCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(namespaceDecl(unless(isAnonymous())).bind("namespace"), this);
}

bool NamespaceNamingCheck::isValidNamespaceName(StringRef Name) const {
  return Name.size() >= 2 && Name[0] == 'N' && std::isupper(Name[1]);
}

std::string NamespaceNamingCheck::toValidNamespaceName(StringRef Name) const {
  if (Name.empty())
    return "N";
  std::string Result = "N";
  size_t Start = (Name[0] == 'N' || Name[0] == 'n') ? 1 : 0;
  bool CapitalizeNext = true;
  for (size_t I = Start; I < Name.size(); ++I) {
    if (Name[I] == '_') {
      CapitalizeNext = true;
      continue;
    }
    Result += static_cast<char>(CapitalizeNext ? std::toupper(Name[I]) : std::tolower(Name[I]));
    CapitalizeNext = false;
  }
  return Result;
}

void NamespaceNamingCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *NS = Result.Nodes.getNodeAs<NamespaceDecl>("namespace");
  if (!NS || NS->isAnonymousNamespace())
    return;
  StringRef Name = NS->getName();
  if (Name == "std" || Name == "boost" || Name.startswith("__") || NS->isInline())
    return;
  if (!isValidNamespaceName(Name)) {
    diag(NS->getLocation(),
         "namespace name '%0' should be in PascalCase and prefixed with 'N'; consider renaming to '%1'")
        << Name << toValidNamespaceName(Name);
  }
}

} // namespace clang::tidy::yt
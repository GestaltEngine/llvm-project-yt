//===--- EnumNamingCheck.cpp - clang-tidy ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "EnumNamingCheck.h"
#include "NamingUtils.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::yt {

void EnumNamingCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(enumDecl(unless(isImplicit())).bind("enum"), this);
}

bool EnumNamingCheck::isValidEnumName(StringRef Name) const {
  return Name.size() >= 2 && Name[0] == 'E' && std::isupper(Name[1]);
}

std::string EnumNamingCheck::toValidEnumName(StringRef Name) const {
  if (Name.empty())
    return "E";

  // Remove 'E' or 'e' prefix if present
  StringRef BaseName = Name;
  if (!Name.empty() && (Name[0] == 'E' || Name[0] == 'e')) {
    BaseName = Name.drop_front(1);
  }

  // Convert to PascalCase and add 'E' prefix
  return "E" + toPascalCase(BaseName);
}

void EnumNamingCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Enum = Result.Nodes.getNodeAs<EnumDecl>("enum");
  if (!Enum || !Enum->getIdentifier())
    return;
  StringRef Name = Enum->getName();
  if (Name.starts_with("__"))
    return;
  if (!isValidEnumName(Name)) {
    diag(Enum->getLocation(),
         "enum name '%0' should be in PascalCase and prefixed with 'E'; consider renaming to '%1'")
        << Name << toValidEnumName(Name);
  }
}

} // namespace clang::tidy::yt
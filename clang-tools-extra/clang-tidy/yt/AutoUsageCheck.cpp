//===--- AutoUsageCheck.cpp - clang-tidy ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "AutoUsageCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::yt {

void AutoUsageCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(varDecl(hasType(autoType()), unless(isImplicit())).bind("var"), this);
}

bool AutoUsageCheck::isIntOrBoolType(QualType Type) const {
  if (Type.isNull()) {
    // has not been deduced yet.
    return false;
  }
  Type = Type.getNonReferenceType().getUnqualifiedType();
  const auto *BT = Type->getAs<BuiltinType>();
  if (!BT)
    return false;
  switch (BT->getKind()) {
    case BuiltinType::Bool:
    case BuiltinType::Int:
        return true;
    default:
        return false;
  }
}

void AutoUsageCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Var = Result.Nodes.getNodeAs<VarDecl>("var");
  if (!Var || !Var->hasInit() || Var->isInitCapture())
    return;
  QualType DeducedType = Var->getType();
  if (const auto *AT = DeducedType->getAs<AutoType>()) {
    if (!AT->isDeduced())
      return;
    DeducedType = AT->getDeducedType();
  }
  if (isIntOrBoolType(DeducedType)) {
    DeducedType = DeducedType.getCanonicalType();
    diag(Var->getLocation(),
         "use of 'auto' with primitive type '%0' is discouraged; prefer explicit type declaration")
        << DeducedType.getAsString();
  }
}

} // namespace clang::tidy::yt
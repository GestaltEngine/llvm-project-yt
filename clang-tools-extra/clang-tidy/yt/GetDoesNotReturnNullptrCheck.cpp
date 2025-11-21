//===--- GetDoesNotReturnNullptrCheck.cpp - clang-tidy -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "GetDoesNotReturnNullptrCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::yt {

namespace {

/// Visitor to check if a function body contains return nullptr
class NullptrReturnVisitor : public RecursiveASTVisitor<NullptrReturnVisitor> {
public:
  bool VisitReturnStmt(ReturnStmt *RS) {
    if (!RS->getRetValue())
      return true;

    Expr *RetValue = RS->getRetValue();

    // Check for nullptr literal (before ignoring casts)
    if (isa<CXXNullPtrLiteralExpr>(RetValue)) {
      FoundNullptrReturn = true;
      return false;
    }

    // Also check after ignoring implicit casts
    RetValue = RetValue->IgnoreParenImpCasts();
    if (isa<CXXNullPtrLiteralExpr>(RetValue)) {
      FoundNullptrReturn = true;
      return false;
    }

    return true;
  }

  bool foundNullptrReturn() const { return FoundNullptrReturn; }

private:
  bool FoundNullptrReturn = false;
};

} // namespace

void GetDoesNotReturnNullptrCheck::registerMatchers(MatchFinder *Finder) {
  // Match functions and methods that:
  // 1. Return a pointer type
  // 2. Have a definition (body)
  Finder->addMatcher(
      functionDecl(
          returns(pointerType()),
          isDefinition(),
          unless(isImplicit())
      ).bind("func"),
      this);
}

void GetDoesNotReturnNullptrCheck::check(
    const MatchFinder::MatchResult &Result) {
  const auto *Func = Result.Nodes.getNodeAs<FunctionDecl>("func");
  if (!Func || !Func->hasBody())
    return;

  // Check if function name starts with "Get"
  StringRef Name = Func->getName();
  if (!Name.starts_with("Get"))
    return;

  // Check if the function body contains return nullptr
  NullptrReturnVisitor Visitor;
  Visitor.TraverseStmt(Func->getBody());

  if (Visitor.foundNullptrReturn()) {
    diag(Func->getLocation(),
         "function '%0' starts with 'Get' but may return nullptr; "
         "consider using 'Find' prefix instead")
        << Name;
  }
}

} // namespace clang::tidy::yt
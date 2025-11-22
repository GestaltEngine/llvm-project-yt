#include "WaitForInConstructorCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::yt {

namespace {

/// Visitor to check if a constructor body contains WaitFor calls
class WaitForCallVisitor : public RecursiveASTVisitor<WaitForCallVisitor> {
public:
  bool VisitCallExpr(CallExpr *CE) {
    if (const FunctionDecl *Callee = CE->getDirectCallee()) {
      // Check if the function name is "WaitFor"
      if (Callee->getName() == "WaitFor") {
        WaitForCalls.push_back(CE);
      }
    }
    return true;
  }

  const std::vector<CallExpr *> &getWaitForCalls() const {
    return WaitForCalls;
  }

private:
  std::vector<CallExpr *> WaitForCalls;
};

} // namespace

void WaitForInConstructorCheck::registerMatchers(MatchFinder *Finder) {
  // Match constructor definitions (not declarations)
  Finder->addMatcher(
      cxxConstructorDecl(
          isDefinition(),
          unless(isImplicit())
      ).bind("ctor"),
      this);
}

void WaitForInConstructorCheck::check(
    const MatchFinder::MatchResult &Result) {
  const auto *Ctor = Result.Nodes.getNodeAs<CXXConstructorDecl>("ctor");
  if (!Ctor)
    return;

  WaitForCallVisitor Visitor;

  // Check constructor initializers (member initializer list)
  for (const auto *Init : Ctor->inits()) {
    if (Init->getInit()) {
      Visitor.TraverseStmt(Init->getInit());
    }
  }

  // Check constructor body
  if (Ctor->hasBody()) {
    Visitor.TraverseStmt(Ctor->getBody());
  }

  for (const auto *WaitForCall : Visitor.getWaitForCalls()) {
    diag(WaitForCall->getBeginLoc(),
         "do not use WaitFor in constructors")
        << WaitForCall->getSourceRange();

    // Add a note pointing to the constructor
    diag(Ctor->getLocation(), "constructor defined here",
         DiagnosticIDs::Note);
  }
}

} // namespace clang::tidy::yt
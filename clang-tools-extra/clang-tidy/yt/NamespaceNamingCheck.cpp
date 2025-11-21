#include "NamespaceNamingCheck.h"
#include "NamingUtils.h"
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

  // Remove 'N' or 'n' prefix if present
  StringRef BaseName = Name;
  if (!Name.empty() && (Name[0] == 'N' || Name[0] == 'n')) {
    BaseName = Name.drop_front(1);
  }

  // Convert to PascalCase and add 'N' prefix
  return "N" + toPascalCase(BaseName);
}

void NamespaceNamingCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *NS = Result.Nodes.getNodeAs<NamespaceDecl>("namespace");
  if (!NS || NS->isAnonymousNamespace())
    return;
  StringRef Name = NS->getName();
  if (Name == "std" || Name == "boost" || Name.starts_with("__") || NS->isInline())
    return;
  if (!isValidNamespaceName(Name)) {
    diag(NS->getLocation(),
         "namespace name '%0' should be in PascalCase and prefixed with 'N'; consider renaming to '%1'")
        << Name << toValidNamespaceName(Name);
  }
}

} // namespace clang::tidy::yt
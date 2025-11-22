#include "TriviallyCopyableParameterByValueCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::yt {

void TriviallyCopyableParameterByValueCheck::registerMatchers(MatchFinder *Finder) {
  // Match function parameters that are:
  // 1. Passed by const reference (const T&)
  // 2. Not in template instantiations (to avoid duplicate warnings)
  Finder->addMatcher(
      parmVarDecl(
          hasType(qualType(
              references(qualType(isConstQualified()))
          )),
          unless(isInstantiated()),
          unless(isImplicit())
      ).bind("param"),
      this);
}

void TriviallyCopyableParameterByValueCheck::check(
    const MatchFinder::MatchResult &Result) {
  const auto *Param = Result.Nodes.getNodeAs<ParmVarDecl>("param");
  if (!Param)
    return;

  QualType ParamType = Param->getType();

  // Get the referenced type (strip the reference)
  if (!ParamType->isReferenceType())
    return;

  QualType ReferencedType = ParamType.getNonReferenceType();

  // Remove const qualifier to get the underlying type
  QualType UnderlyingType = ReferencedType.getUnqualifiedType();

  const ASTContext &Context = *Result.Context;

  // Exclude copy constructors and copy assignment operators
  if (const auto *Method = dyn_cast_or_null<CXXMethodDecl>(Param->getDeclContext())) {
    if (const auto *Ctor = dyn_cast<CXXConstructorDecl>(Method)) {
      if (Ctor->isCopyConstructor()) {
        return;
      }
    }
    if (Method->isCopyAssignmentOperator()) {
      return;
    }
  }

  // Check if the type is trivially copyable
  if (!UnderlyingType.isTriviallyCopyableType(Context))
    return;

  // Check the size of the type
  // We want types that are 16 bytes or less
  uint64_t TypeSize = Context.getTypeSize(UnderlyingType) / 8; // Convert bits to bytes

  if (TypeSize > 16)
    return;

  // Get the type name for the diagnostic message
  std::string TypeName = UnderlyingType.getAsString();

  diag(Param->getLocation(),
       "parameter '%0' of trivially copyable type '%1' (%2 bytes) "
       "should be passed by value, not by const reference")
      << Param->getName() << TypeName << TypeSize;
}

} // namespace clang::tidy::yt
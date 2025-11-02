#include "ClassNamingCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include <cctype>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace yt {

namespace {

bool isPascalCase(StringRef Name) {
  if (Name.empty())
    return false;

  // First character should be uppercase
  if (!std::isupper(Name[0]))
    return false;

  if (Name.size() <= 3) {
    return true;
  }

  for (char C : Name) {
    if (std::islower(C)) {
      return true;
    }
  }

  return false;
}

bool isInterfaceBaseName(StringRef BaseName) {
  return BaseName[0] == 'I';
}

bool isTRefCountedBase(StringRef BaseName) {
  return BaseName.endswith("TRefCounted");
}

bool isInterface(const CXXRecordDecl *Record) {
  // All explicit methods should be pure virtual
  for (const auto *Method : Record->methods()) {
    if (Method->isImplicit()) {
      continue;
    }
    if (!Method->isVirtual() || !Method->isPure()) {
      return false;
    }
  }

  // Check all base classes - they should be either interfaces or TRefCounted
  for (const auto &Base : Record->bases()) {
    std::string BaseTypeName = Base.getType().getAsString();
    if (isTRefCountedBase(BaseTypeName)) {
      continue;
    }

    // Extract just the class name (remove namespace qualifiers)
    size_t LastColon = BaseTypeName.rfind("::");
    if (LastColon != std::string::npos)
      BaseTypeName = BaseTypeName.substr(LastColon + 2);

    // Base should be either an interface (starts with I)
    if (!isInterfaceBaseName(BaseTypeName)) {
      return false;
    }
  }

  return true;
}

} // namespace

void ClassNamingCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(cxxRecordDecl(isDefinition()).bind("record"), this);
  Finder->addMatcher(enumDecl().bind("enum"), this);
}

void ClassNamingCheck::check(const MatchFinder::MatchResult &Result) {
  if (const auto *Record = Result.Nodes.getNodeAs<CXXRecordDecl>("record")) {
    StringRef Name = Record->getName();

    // Skip anonymous records
    if (Record->isAnonymousStructOrUnion() || Name.empty())
      return;

    if (Record->isAbstract()) {
      if (isInterface(Record)) {
        if (!Record->isStruct()) {
          diag(Record->getLocation(), "interfaces should be structs");
        }
        if (!Name.startswith("I") || !isPascalCase(Name.drop_front(1))) {
          diag(Record->getLocation(), "interface name '%0' should start with 'I' and be in PascalCase")
              << Name;
        }
      } else {
        if (!Record->isClass()) {
          diag(Record->getLocation(), "abstract structs which are not interface-like should be classes");
        }
        if (!Name.startswith("T") || !isPascalCase(Name.drop_front(1))) {
          diag(Record->getLocation(), "abstract class name '%0' should start with 'T' and be in PascalCase")
              << Name;
        }
      }
    }
  }
}

} // namespace yt
} // namespace tidy
} // namespace clang
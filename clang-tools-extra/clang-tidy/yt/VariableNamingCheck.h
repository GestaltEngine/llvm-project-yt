//===--- VariableNamingCheck.h - clang-tidy ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_VARIABLENAMING_CHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_VARIABLENAMING_CHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::yt {

class VariableNamingCheck : public ClangTidyCheck {
public:
  VariableNamingCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  bool isCamelCase(StringRef Name) const;
  bool isPascalCase(StringRef Name) const;
  std::string toCamelCase(StringRef Name) const;
  std::string toPascalCase(StringRef Name) const;
};

} // namespace clang::tidy::yt

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_VARIABLENAMING_CHECK_H
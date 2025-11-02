//===--- EnumNamingCheck.h - clang-tidy -------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_ENUMNAMING_CHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_ENUMNAMING_CHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::yt {

class EnumNamingCheck : public ClangTidyCheck {
public:
  EnumNamingCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  bool isValidEnumName(StringRef Name) const;
  std::string toValidEnumName(StringRef Name) const;
};

} // namespace clang::tidy::yt

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_ENUMNAMING_CHECK_H
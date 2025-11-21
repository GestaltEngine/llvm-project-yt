//===--- NamingUtils.cpp - clang-tidy -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "NamingUtils.h"
#include <cctype>

namespace clang::tidy::yt {

bool isCamelCase(llvm::StringRef Name) {
  return !Name.empty() && std::islower(static_cast<unsigned char>(Name[0]));
}

bool isPascalCase(llvm::StringRef Name) {
  return !Name.empty() && std::isupper(static_cast<unsigned char>(Name[0]));
}

std::string toCamelCase(llvm::StringRef Name) {
  if (Name.empty())
    return "";

  std::string Result;
  Result.reserve(Name.size());

  bool IsFirstChar = true;
  bool CapitalizeNext = false;
  bool PrevWasUpper = false;

  for (char C : Name) {
     // Handle underscores (snake_case separator)
    if (C == '_') {
      CapitalizeNext = true;
      PrevWasUpper = false;
      continue;
    }

    bool IsUpper = std::isupper(static_cast<unsigned char>(C));

    if (IsFirstChar) {
      // First character should always be lowercase
      Result += static_cast<char>(std::tolower(static_cast<unsigned char>(C)));
      IsFirstChar = false;
      PrevWasUpper = IsUpper;
    } else if (CapitalizeNext) {
      // After underscore, capitalize
      Result += static_cast<char>(std::toupper(static_cast<unsigned char>(C)));
      CapitalizeNext = false;
      PrevWasUpper = true;
    } else if (IsUpper) {
      // Handle PascalCase -> camelCase conversion
      // Only convert to lowercase if previous char was also uppercase (UPPER_CASE)
      // Keep uppercase if previous was lowercase (PascalCase -> pascalCase)
      if (PrevWasUpper) {
        Result += static_cast<char>(std::tolower(static_cast<unsigned char>(C)));
      } else {
        Result += C;
      }
      PrevWasUpper = true;
    } else {
      // Lowercase letter, keep as is
      Result += C;
      PrevWasUpper = false;
    }
  }

  return Result;
}

std::string toPascalCase(llvm::StringRef Name) {
  if (Name.empty())
    return "";

  std::string Result;
  Result.reserve(Name.size());

  bool CapitalizeNext = true;
  bool PrevWasUpper = false;

  for (char C : Name) {
     // Handle underscores (snake_case separator)
    if (C == '_') {
      CapitalizeNext = true;
      PrevWasUpper = false;
      continue;
    }

    bool IsUpper = std::isupper(static_cast<unsigned char>(C));

    if (CapitalizeNext) {
      // Capitalize after underscore or at start
      Result += static_cast<char>(std::toupper(static_cast<unsigned char>(C)));
      CapitalizeNext = false;
      PrevWasUpper = true;
    } else if (IsUpper) {
      // Handle consecutive uppercase letters (acronyms)
      // Only convert to lowercase if previous char was also uppercase (UPPER_CASE)
      // Keep uppercase if previous was lowercase (already PascalCase)
      if (PrevWasUpper) {
        Result += static_cast<char>(std::tolower(static_cast<unsigned char>(C)));
      } else {
        Result += C;
      }
      PrevWasUpper = true;
    } else {
      // Lowercase or non-alphabetic character, keep as is
      Result += C;
      PrevWasUpper = false;
    }
  }

  return Result;
}

} // namespace clang::tidy::yt
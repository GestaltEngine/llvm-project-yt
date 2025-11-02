//===--- NamingUtils.h - clang-tidy -----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_NAMINGUTILS_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_NAMINGUTILS_H

#include "llvm/ADT/StringRef.h"
#include <string>

namespace clang::tidy::yt {

/// Check if a name is in camelCase format (starts with lowercase).
bool isCamelCase(llvm::StringRef Name);

/// Check if a name is in PascalCase format (starts with uppercase).
bool isPascalCase(llvm::StringRef Name);

/// Convert a name from PascalCase or snake_case to camelCase.
/// Examples:
///   "PascalCase" -> "pascalCase"
///   "snake_case" -> "snakeCase"
///   "UPPER_CASE" -> "upperCase"
std::string toCamelCase(llvm::StringRef Name);

/// Convert a name from camelCase or snake_case to PascalCase.
/// Examples:
///   "camelCase" -> "CamelCase"
///   "snake_case" -> "SnakeCase"
///   "UPPER_CASE" -> "UpperCase"
std::string toPascalCase(llvm::StringRef Name);

} // namespace clang::tidy::yt

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_NAMINGUTILS_H
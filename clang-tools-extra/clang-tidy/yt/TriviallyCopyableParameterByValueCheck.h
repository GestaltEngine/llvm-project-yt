#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_TRIVIALLYCOPIABLEPARAMETERBYVALUE_CHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_TRIVIALLYCOPIABLEPARAMETERBYVALUE_CHECK_H

#include "../ClangTidyCheck.h"
#include "llvm/Support/Regex.h"
#include <string>

namespace clang::tidy::yt {

/// Checks that trivially copyable types 16 bytes or less are passed by value,
/// not by reference.
///
/// This check detects function parameters that:
/// - Are passed by const reference (const T&)
/// - Have a trivially copyable type
/// - Are 16 bytes or less in size
///
/// For example:
/// \code
///   using TTransactionId = TGuid;
///   void DoFinishShuffle(const TTransactionId& transactionId); // Bad
///   void DoFinishShuffle(TTransactionId transactionId);        // Good
/// \endcode

class TriviallyCopyableParameterByValueCheck : public ClangTidyCheck {
public:
  TriviallyCopyableParameterByValueCheck(StringRef Name, ClangTidyContext *Context);
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
  void storeOptions(ClangTidyOptions::OptionMap &Opts) override;

private:
  // Store both compiled and non-compiled forms so original value can be
  // serialized
  std::string IgnoredTypesRegexStr;
  llvm::Regex IgnoredTypesRegex;
};

} // namespace clang::tidy::yt

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_TRIVIALLYCOPIABLEPARAMETERBYVALUE_CHECK_H
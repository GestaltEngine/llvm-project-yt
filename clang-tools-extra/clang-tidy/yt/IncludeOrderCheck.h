#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_INCLUDEORDERCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_INCLUDEORDERCHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::yt {

/// Checks the correct order of `#includes` for YTsaurus projects.
///
/// The expected order is:
/// 1. Local headers (non-angled includes)
/// 2. yt/yt/server/...
/// 3. yt/yt/ytlib/...
/// 4. yt/yt/client/... or yt/yt/library/... (possibly intermixed)
/// 5. yt/yt/core/...
/// 6. library/...
/// 7. util/...
/// 8. Other arcadia includes
/// 9. Standard C/C++ library includes (angled includes)
class IncludeOrderCheck : public ClangTidyCheck {
public:
  IncludeOrderCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerPPCallbacks(const SourceManager &SM, Preprocessor *PP,
                           Preprocessor *ModuleExpanderPP) override;
};

} // namespace clang::tidy::yt

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_YT_INCLUDEORDERCHECK_H
#include "../ClangTidy.h"
#include "../ClangTidyModule.h"
#include "../ClangTidyModuleRegistry.h"
#include "AutoUsageCheck.h"
#include "ClassNamingCheck.h"
#include "EnumNamingCheck.h"
#include "FunctionNamingCheck.h"
#include "GetDoesNotReturnNullptrCheck.h"
#include "IncludeOrderCheck.h"
#include "NamespaceNamingCheck.h"
#include "TestNamingCheck.h"
#include "TriviallyCopyableParameterByValueCheck.h"
#include "VariableNamingCheck.h"

namespace clang {
namespace tidy {
namespace yt {

class YTModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<AutoUsageCheck>("yt-auto-usage");
    CheckFactories.registerCheck<ClassNamingCheck>("yt-class-naming");
    CheckFactories.registerCheck<EnumNamingCheck>("yt-enum-naming");
    CheckFactories.registerCheck<FunctionNamingCheck>("yt-function-naming");
    CheckFactories.registerCheck<GetDoesNotReturnNullptrCheck>("yt-get-does-not-return-nullptr");
    CheckFactories.registerCheck<IncludeOrderCheck>("yt-include-order");
    CheckFactories.registerCheck<NamespaceNamingCheck>("yt-namespace-naming");
    CheckFactories.registerCheck<TestNamingCheck>("yt-test-naming");
    CheckFactories.registerCheck<TriviallyCopyableParameterByValueCheck>("yt-trivially-copyable-parameter-by-value");
    CheckFactories.registerCheck<VariableNamingCheck>("yt-variable-naming");
  }
};

} // namespace yt
} // namespace tidy
} // namespace clang

namespace clang {
namespace tidy {

// Register the YTTidyModule using this statically initialized variable.
static ClangTidyModuleRegistry::Add<yt::YTModule>
    X("yt-module", "Adds YTsaurus-specific checks.");

// This anchor is used to force the linker to link in the generated object file
// and thus register the YTModule.
volatile int YTModuleAnchorSource = 0;

} // namespace tidy
} // namespace clang
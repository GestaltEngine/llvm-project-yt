#include "ClangTidyTest.h"
#include "google/ExplicitConstructorCheck.h"
#include "google/GlobalNamesInHeadersCheck.h"
#include "yt/ClassNamingCheck.h"
#include "yt/NamespaceNamingCheck.h"
#include "gtest/gtest.h"

using namespace clang::tidy::google;

namespace clang {
namespace tidy {
namespace test {

class ClassNamesCheckTest : public ::testing::Test {
protected:
  bool runCheckOnCode(const std::string &Code) {
    static const char Filename[] = "test.cpp";
    std::vector<ClangTidyError> Errors;
    std::vector<std::string> Args;
    test::runCheckOnCode<yt::ClassNamingCheck>(Code, &Errors, Filename, Args);
    if (Errors.empty())
      return false;
    return true;
  }
};

TEST_F(ClassNamesCheckTest, Abstruct1) {
  EXPECT_FALSE(runCheckOnCode("class TRefCounted { };\n"
                              "struct ILululu : public TRefCounted { };"));
}

TEST_F(ClassNamesCheckTest, Abstruct2) {
  EXPECT_FALSE(runCheckOnCode("class TRefCounted { };\n"
                              "struct ILululu : public ::TRefCounted { };"));
}

TEST_F(ClassNamesCheckTest, Abstruct3) {
  EXPECT_FALSE(runCheckOnCode("namespace NYT { class TRefCounted { }; }\n"
                              "struct ILululu : public ::NYT::TRefCounted { };"));
}

TEST_F(ClassNamesCheckTest, Abstruct4) {
  EXPECT_FALSE(runCheckOnCode("namespace NYT { class TRefCounted { }; }\n"
                              "namespace NSpace { struct IOther { }; }\n"
                              "struct ILululu : public NSpace::IOther, public NYT::TRefCounted\n"
                              "{\n"
                              "    virtual void Foo() = 0;\n"
                              "};"));
}

TEST_F(ClassNamesCheckTest, Abstruct5) {
  EXPECT_FALSE(runCheckOnCode("namespace NYT { class TRefCounted { }; }\n"
                              "struct IOther { };\n"
                              "struct ILululu : public IOther, public NYT::TRefCounted\n"
                              "{\n"
                              "    virtual void Foo() = 0;\n"
                              "};"));
}

TEST_F(ClassNamesCheckTest, Abstruct6) {
  EXPECT_FALSE(runCheckOnCode("namespace NYT { class TRefCounted { }; }\n"
                              "namespace NSpace { struct IOther { }; }\n"
                              "struct ILululu : public NSpace::IOther, public NYT::TRefCounted\n"
                              "{\n"
                              "    virtual void Foo() = 0;\n"
                              "};"));
}

class NamespaceNamingCheckTest : public ::testing::Test {
protected:
  bool runCheckOnCode(const std::string &Code) {
    static const char Filename[] = "test.cpp";
    std::vector<ClangTidyError> Errors;
    std::vector<std::string> Args;
    test::runCheckOnCode<yt::NamespaceNamingCheck>(Code, &Errors, Filename, Args);
    if (Errors.empty())
      return false;
    return true;
  }
};

// Test valid namespace names (should not trigger warnings)
TEST_F(NamespaceNamingCheckTest, ValidNamespaceNames) {
  EXPECT_FALSE(runCheckOnCode("namespace NYT { }"));
  EXPECT_FALSE(runCheckOnCode("namespace NUtils { }"));
  EXPECT_FALSE(runCheckOnCode("namespace NMyNamespace { }"));
  EXPECT_FALSE(runCheckOnCode("namespace NFoo { }"));
}

// Test invalid namespace names (should trigger warnings)
TEST_F(NamespaceNamingCheckTest, InvalidNamespaceNames) {
  EXPECT_TRUE(runCheckOnCode("namespace yt { }"));
  EXPECT_TRUE(runCheckOnCode("namespace my_namespace { }"));
  EXPECT_TRUE(runCheckOnCode("namespace myNamespace { }"));
  EXPECT_TRUE(runCheckOnCode("namespace Foo { }"));
  EXPECT_TRUE(runCheckOnCode("namespace nFoo { }"));
}

// Test namespace names starting with 'N' but not followed by uppercase
TEST_F(NamespaceNamingCheckTest, InvalidNPrefix) {
  EXPECT_TRUE(runCheckOnCode("namespace Nfoo { }"));
  EXPECT_TRUE(runCheckOnCode("namespace nFoo { }"));
  EXPECT_TRUE(runCheckOnCode("namespace N { }"));
}

// Test nested namespaces
TEST_F(NamespaceNamingCheckTest, NestedNamespaces) {
  EXPECT_FALSE(runCheckOnCode("namespace NYT { namespace NUtils { } }"));
  EXPECT_TRUE(runCheckOnCode("namespace NYT { namespace utils { } }"));
  EXPECT_TRUE(runCheckOnCode("namespace yt { namespace NUtils { } }"));
}

// Test anonymous namespaces (should be ignored)
TEST_F(NamespaceNamingCheckTest, AnonymousNamespace) {
  EXPECT_FALSE(runCheckOnCode("namespace { void foo() {} }"));
  EXPECT_FALSE(runCheckOnCode("namespace NYT { namespace { void foo() {} } }"));
}

// Test standard library namespaces (should be ignored)
TEST_F(NamespaceNamingCheckTest, StandardNamespaces) {
  EXPECT_FALSE(runCheckOnCode("namespace std { }"));
  EXPECT_FALSE(runCheckOnCode("namespace boost { }"));
}

// Test inline namespaces (should be ignored)
TEST_F(NamespaceNamingCheckTest, InlineNamespace) {
  EXPECT_FALSE(runCheckOnCode("inline namespace v1 { }"));
  EXPECT_FALSE(runCheckOnCode("namespace NYT { inline namespace v1 { } }"));
}

// Test namespaces with double underscore prefix (should be ignored)
TEST_F(NamespaceNamingCheckTest, DoubleUnderscorePrefix) {
  EXPECT_FALSE(runCheckOnCode("namespace __detail { }"));
  EXPECT_FALSE(runCheckOnCode("namespace __internal { }"));
}

// Test complex nested structure
TEST_F(NamespaceNamingCheckTest, ComplexNesting) {
  EXPECT_FALSE(runCheckOnCode(
      "namespace NYT {\n"
      "  namespace NUtils {\n"
      "    namespace NDetail {\n"
      "      void foo() {}\n"
      "    }\n"
      "  }\n"
      "}"));
}

// Test mixed valid and invalid nested namespaces
TEST_F(NamespaceNamingCheckTest, MixedNesting) {
  EXPECT_TRUE(runCheckOnCode(
      "namespace NYT {\n"
      "  namespace utils {\n"
      "    void foo() {}\n"
      "  }\n"
      "}"));
  EXPECT_TRUE(runCheckOnCode(
      "namespace NYT::utils {\n"
      "}"));
}

// Test namespace with classes
TEST_F(NamespaceNamingCheckTest, NamespaceWithClasses) {
  EXPECT_FALSE(runCheckOnCode(
      "namespace NYT {\n"
      "  class TFoo { };\n"
      "}"));
  EXPECT_TRUE(runCheckOnCode(
      "namespace yt {\n"
      "  class TFoo { };\n"
      "}"));
}

} // namespace test
} // namespace tidy
} // namespace clang

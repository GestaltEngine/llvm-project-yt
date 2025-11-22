#include "ClangTidyTest.h"
#include "google/ExplicitConstructorCheck.h"
#include "google/GlobalNamesInHeadersCheck.h"
#include "yt/ClassNamingCheck.h"
#include "yt/NamespaceNamingCheck.h"
#include "yt/GetDoesNotReturnNullptrCheck.h"
#include "yt/TestNamingCheck.h"
#include "yt/TriviallyCopyableParameterByValueCheck.h"
#include "yt/WaitForInConstructorCheck.h"
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
class GetDoesNotReturnNullptrCheckTest : public ::testing::Test {
protected:
  bool runCheckOnCode(const std::string &Code) {
    static const char Filename[] = "test.cpp";
    std::vector<ClangTidyError> Errors;
    std::vector<std::string> Args;
    test::runCheckOnCode<yt::GetDoesNotReturnNullptrCheck>(Code, &Errors, Filename, Args);
    if (Errors.empty())
      return false;
    return true;
  }
};

// Test function starting with Get that returns nullptr (should trigger warning)
TEST_F(GetDoesNotReturnNullptrCheckTest, GetReturnsNullptr) {
  EXPECT_TRUE(runCheckOnCode(
      "class TKey { };\n"
      "TKey* GetKey(int id) {\n"
      "  if (id < 0) {\n"
      "    return nullptr;\n"
      "  }\n"
      "  return new TKey();\n"
      "}"));
}

// Test method starting with Get that returns nullptr (should trigger warning)
TEST_F(GetDoesNotReturnNullptrCheckTest, GetMethodReturnsNullptr) {
  EXPECT_TRUE(runCheckOnCode(
      "class TKey { };\n"
      "class TOwner {\n"
      "public:\n"
      "  TKey* GetKey(int id) {\n"
      "    if (id < 0) {\n"
      "      return nullptr;\n"
      "    }\n"
      "    return new TKey();\n"
      "  }\n"
      "};"));
}

// Test function starting with Get that never returns nullptr (should not trigger)
TEST_F(GetDoesNotReturnNullptrCheckTest, GetNeverReturnsNullptr) {
  EXPECT_FALSE(runCheckOnCode(
      "class TKey { };\n"
      "TKey* GetKey(int id) {\n"
      "  return new TKey();\n"
      "}"));
}

// Test function starting with Find that returns nullptr (should not trigger)
TEST_F(GetDoesNotReturnNullptrCheckTest, FindReturnsNullptr) {
  EXPECT_FALSE(runCheckOnCode(
      "class TKey { };\n"
      "TKey* FindKey(int id) {\n"
      "  if (id < 0) {\n"
      "    return nullptr;\n"
      "  }\n"
      "  return new TKey();\n"
      "}"));
}

// Test function not starting with Get (should not trigger)
TEST_F(GetDoesNotReturnNullptrCheckTest, NonGetFunction) {
  EXPECT_FALSE(runCheckOnCode(
      "class TKey { };\n"
      "TKey* CreateKey(int id) {\n"
      "  if (id < 0) {\n"
      "    return nullptr;\n"
      "  }\n"
      "  return new TKey();\n"
      "}"));
}

// Test function starting with Get but not returning pointer (should not trigger)
TEST_F(GetDoesNotReturnNullptrCheckTest, GetReturnsNonPointer) {
  EXPECT_FALSE(runCheckOnCode(
      "int GetValue(int id) {\n"
      "  return id;\n"
      "}"));
}

// Test function declaration without definition (should not trigger)
TEST_F(GetDoesNotReturnNullptrCheckTest, GetDeclarationOnly) {
  EXPECT_FALSE(runCheckOnCode(
      "class TKey { };\n"
      "TKey* GetKey(int id);"));
}

class TestNamingCheckTest : public ::testing::Test {
protected:
  bool runCheckOnCode(const std::string &Code) {
    static const char Filename[] = "test.cpp";
    std::vector<ClangTidyError> Errors;
    std::vector<std::string> Args{"-std=c++17"};
    test::runCheckOnCode<yt::TestNamingCheck>(Code, &Errors, Filename, Args);
    if (Errors.empty())
      return false;
    return true;
  }
};

// Test valid test naming (should not trigger warnings)
TEST_F(TestNamingCheckTest, ValidTestNaming) {
  EXPECT_FALSE(runCheckOnCode(
      "#define TEST_F(fixture, name) void fixture##_##name()\n"
      "TEST_F(TPartitionKeysBuilderTest, TwoPartitions);"));
}

// Test invalid test fixture name (should trigger warning)
TEST_F(TestNamingCheckTest, InvalidFixtureName) {
  EXPECT_TRUE(runCheckOnCode(
      "#define TEST_F(fixture, name) void fixture##_##name()\n"
      "TEST_F(TPartitionKeysBuilder, TwoPartitions);"));
}

// Test invalid test method name with Test prefix (should trigger warning)
TEST_F(TestNamingCheckTest, InvalidTestMethodName) {
  EXPECT_TRUE(runCheckOnCode(
      "#define TEST_F(fixture, name) void fixture##_##name()\n"
      "TEST_F(TPartitionKeysBuilderTest, TestTwoPartitions);"));
}

// Test both invalid fixture and method names (should trigger both warnings)
TEST_F(TestNamingCheckTest, BothInvalid) {
  EXPECT_TRUE(runCheckOnCode(
      "#define TEST_F(fixture, name) void fixture##_##name()\n"
      "TEST_F(TPartitionKeysBuilder, TestTwoPartitions);"));
}

// Test TEST_P macro (should work the same way)
TEST_F(TestNamingCheckTest, ValidTestPNaming) {
  EXPECT_FALSE(runCheckOnCode(
      "#define TEST_P(fixture, name) void fixture##_##name()\n"
      "TEST_P(TPartitionKeysBuilderTest, TwoPartitions);"));
}

TEST_F(TestNamingCheckTest, InvalidTestPMethodName) {
  EXPECT_TRUE(runCheckOnCode(
      "#define TEST_P(fixture, name) void fixture##_##name()\n"
      "TEST_P(TPartitionKeysBuilderTest, TestTwoPartitions);"));
}

class TriviallyCopyableParameterByValueCheckTest : public ::testing::Test {
protected:
  bool runCheckOnCode(const std::string &Code) {
    static const char Filename[] = "test.cpp";
    std::vector<ClangTidyError> Errors;
    std::vector<std::string> Args{"-std=c++17"};
    test::runCheckOnCode<yt::TriviallyCopyableParameterByValueCheck>(Code, &Errors, Filename, Args);
    if (Errors.empty())
      return false;
    return true;
  }
};

// Test that struct declarations within classes do NOT trigger warning (no parameters)
TEST_F(TriviallyCopyableParameterByValueCheckTest, NestedStructDeclaration) {
  EXPECT_FALSE(runCheckOnCode(
      "class TFoo {\n"
      "  struct TModificationData {\n"
      "    int x;\n"
      "    int y;\n"
      "  };\n"
      "};"));
}

// Methods within nested structs should trigger warning (except copy ctor/assignment)
TEST_F(TriviallyCopyableParameterByValueCheckTest, NestedStructDeclarationWithMethod) {
  EXPECT_TRUE(runCheckOnCode(
      "class TFoo {\n"
      "  struct TModificationData {\n"
      "    int x;\n"
      "    int y;\n"
      "    void DoSomething(const TModificationData& data);\n"
      "  };\n"
      "};"));
}

// Test that regular functions with small trivially copyable types DO trigger warning
TEST_F(TriviallyCopyableParameterByValueCheckTest, RegularFunctionSmallType) {
  EXPECT_TRUE(runCheckOnCode(
      "struct TModificationData {\n"
      "  int x;\n"
      "  int y;\n"
      "};\n"
      "void DoSomething(const TModificationData& data) {}"));
}

// Test that large types do not trigger warning
TEST_F(TriviallyCopyableParameterByValueCheckTest, LargeType) {
  EXPECT_FALSE(runCheckOnCode(
      "struct TLargeData {\n"
      "  int data[10];\n"
      "};\n"
      "void DoSomething(const TLargeData& data) {}"));
}

// Test that copy constructors do NOT trigger warning
TEST_F(TriviallyCopyableParameterByValueCheckTest, CopyConstructor) {
  EXPECT_FALSE(runCheckOnCode(
      "struct TModificationData {\n"
      "  int x;\n"
      "  int y;\n"
      "  TModificationData(const TModificationData& other);\n"
      "};"));
}

// Test that copy assignment operators do NOT trigger warning
TEST_F(TriviallyCopyableParameterByValueCheckTest, CopyAssignmentOperator) {
  EXPECT_FALSE(runCheckOnCode(
      "struct TModificationData {\n"
      "  int x;\n"
      "  int y;\n"
      "  TModificationData& operator=(const TModificationData& other);\n"
      "};"));
}
class WaitForInConstructorCheckTest : public ::testing::Test {
protected:
  bool runCheckOnCode(const std::string &Code) {
    static const char Filename[] = "test.cpp";
    std::vector<ClangTidyError> Errors;
    std::vector<std::string> Args{"-std=c++17"};
    test::runCheckOnCode<yt::WaitForInConstructorCheck>(Code, &Errors, Filename, Args);
    if (Errors.empty())
      return false;
    return true;
  }
};

// Test constructor with WaitFor call (should trigger warning)
TEST_F(WaitForInConstructorCheckTest, ConstructorWithWaitFor) {
  EXPECT_TRUE(runCheckOnCode(
      "template<typename T> T WaitFor(T value) { return value; }\n"
      "class TClient { public: int CreateNode() { return 42; } };\n"
      "class TCypressKeyWriter {\n"
      "public:\n"
      "  TCypressKeyWriter(int owner, TClient* client)\n"
      "    : Owner_(owner), Client_(client)\n"
      "  {\n"
      "    auto nodeId = WaitFor(Client_->CreateNode());\n"
      "  }\n"
      "private:\n"
      "  int Owner_;\n"
      "  TClient* Client_;\n"
      "};"));
}

// Test constructor without WaitFor call (should not trigger)
TEST_F(WaitForInConstructorCheckTest, ConstructorWithoutWaitFor) {
  EXPECT_FALSE(runCheckOnCode(
      "class TClient { public: int CreateNode() { return 42; } };\n"
      "class TCypressKeyWriter {\n"
      "public:\n"
      "  TCypressKeyWriter(int owner, TClient* client)\n"
      "    : Owner_(owner), Client_(client)\n"
      "  {\n"
      "    auto nodeId = Client_->CreateNode();\n"
      "  }\n"
      "private:\n"
      "  int Owner_;\n"
      "  TClient* Client_;\n"
      "};"));
}

// Test constructor with WaitFor in member initializer (should trigger warning)
TEST_F(WaitForInConstructorCheckTest, ConstructorWithWaitForInInitializer) {
  EXPECT_TRUE(runCheckOnCode(
      "template<typename T> T WaitFor(T value) { return value; }\n"
      "class TClient { public: int CreateNode() { return 42; } };\n"
      "class TCypressKeyWriter {\n"
      "public:\n"
      "  TCypressKeyWriter(int owner, TClient* client)\n"
      "    : Owner_(owner)\n"
      "    , Client_(client)\n"
      "    , NodeId_(WaitFor(client->CreateNode()))\n"
      "  {}\n"
      "private:\n"
      "  int Owner_;\n"
      "  TClient* Client_;\n"
      "  int NodeId_;\n"
      "};"));
}

// Test regular function with WaitFor (should not trigger)
TEST_F(WaitForInConstructorCheckTest, RegularFunctionWithWaitFor) {
  EXPECT_FALSE(runCheckOnCode(
      "template<typename T> T WaitFor(T value) { return value; }\n"
      "class TClient { public: int CreateNode() { return 42; } };\n"
      "void CreateWriter(TClient* client) {\n"
      "  auto nodeId = WaitFor(client->CreateNode());\n"
      "}"));
}

// Test default constructor (should not trigger)
TEST_F(WaitForInConstructorCheckTest, DefaultConstructor) {
  EXPECT_FALSE(runCheckOnCode(
      "class TCypressKeyWriter {\n"
      "public:\n"
      "  TCypressKeyWriter() = default;\n"
      "};"));
}


} // namespace tidy
} // namespace clang

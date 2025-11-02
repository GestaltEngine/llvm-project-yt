#include "ClangTidyTest.h"
#include "google/ExplicitConstructorCheck.h"
#include "google/GlobalNamesInHeadersCheck.h"
#include "yt/ClassNamingCheck.h"
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

} // namespace test
} // namespace tidy
} // namespace clang

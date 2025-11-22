set(LLVM_LINK_COMPONENTS
  Support
  )

add_clang_library(clangTidyYTModule
  AutoUsageCheck.cpp
  ClassNamingCheck.cpp
  EnumNamingCheck.cpp
  FunctionNamingCheck.cpp
  GetDoesNotReturnNullptrCheck.cpp
  IncludeOrderCheck.cpp
  NamespaceNamingCheck.cpp
  NamingUtils.cpp
  TestNamingCheck.cpp
  TriviallyCopyableParameterByValueCheck.cpp
  VariableNamingCheck.cpp
  WaitForInConstructorCheck.cpp
  YTTidyModule.cpp

  LINK_LIBS
  clangTidy
  clangTidyUtils

  DEPENDS
  omp_gen
  )

clang_target_link_libraries(clangTidyYTModule
  PRIVATE
  clangAST
  clangASTMatchers
  clangBasic
  clangLex
  )
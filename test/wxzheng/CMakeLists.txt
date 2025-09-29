include(FetchContent)

# 3.1 自动下载 gtest（国内可以换 gitee 镜像）
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# 3.2 编译测试可执行文件
add_executable(wxzheng test.cpp
        ../../src/lexer.cpp
        ../../src/token.cpp
        ../../src/ast.h
        ../../src/lexer.h
        ../../src/token.h
        ../../src/parser.cpp
        ../../src/parser.h
        ../../src/ast.cpp
        ../../src/type.cpp
        ../../src/type.h)

# 3.3 链接被测库 + gtest
target_link_libraries(wxzheng
    PRIVATE
    gtest_main          # 自带 main 函数，不用自己写
)

# 3.4 把测试注册到 CTest
add_test(NAME wxzheng COMMAND wxzheng)
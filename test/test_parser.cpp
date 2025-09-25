#include "../src/parser.h"
#include<iostream>
// #include "../cmake-build-debug/_deps/googletest-src/googletest/include/gtest/gtest.h"
#include <gtest/gtest.h>

void runParser(std::string path)
{
    freopen(path.c_str(),"r",stdin);
    int in;
    std::string code;
    while((in=std::cin.get())!=EOF)
        code.push_back(static_cast<char>(in));
    code.push_back('\n');
    parser(code).solve();
}

TEST(Parser, IF1)
{
    EXPECT_NO_THROW(runParser("parser/if1.in"));
}

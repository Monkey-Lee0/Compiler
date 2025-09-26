#include "../src/parser.h"
#include "../src/type.h"
#include<iostream>
#include <gtest/gtest.h>

std::string openFile(std::string path)
{
    path="../../test/testcases/"+path;
    freopen(path.c_str(),"r",stdin);
    int in;
    std::string code;
    while((in=std::cin.get())!=EOF)
        code.push_back(static_cast<char>(in));
    fclose(stdin);
    std::cin.clear();
    code.push_back('\n');
    return code;
}

void runParser(std::string path)
{
    const auto code=openFile(path);
    parser(code).solve();
}

void runSemantic(std::string path)
{
    const auto code=openFile(path);
    semanticCheckType(parser(code).solve());
}

TEST(Parser, IF1){EXPECT_NO_THROW(runParser("if1.in"));}
TEST(Parser, LOOP1){EXPECT_NO_THROW(runParser("loop1.in"));}
TEST(Parser, LOOP2){EXPECT_NO_THROW(runParser("loop2.in"));}
TEST(Parser, LOOP3){EXPECT_NO_THROW(runParser("loop3.in"));}
TEST(Parser, LOOP4){EXPECT_NO_THROW(runParser("loop4.in"));}
TEST(Parser, LOOP5){EXPECT_NO_THROW(runParser("loop5.in"));}
TEST(Parser, LOOP6){EXPECT_NO_THROW(runParser("loop6.in"));}
TEST(Parser, TYPE1){EXPECT_NO_THROW(runParser("type1.in"));}
TEST(Parser, TYPE2){EXPECT_NO_THROW(runParser("type2.in"));}
TEST(Parser, EXPR1){EXPECT_NO_THROW(runParser("expr1.in"));}
TEST(Parser, EXPR2){EXPECT_ANY_THROW(runParser("expr2.in"));}
TEST(Parser, EXPR3){EXPECT_NO_THROW(runParser("expr3.in"));}

TEST(Semantic, IF1){EXPECT_NO_THROW(runSemantic("if1.in"));}
TEST(Semantic, LOOP1){EXPECT_ANY_THROW(runSemantic("loop1.in"));}
TEST(Semantic, LOOP2){EXPECT_NO_THROW(runSemantic("loop2.in"));}
TEST(Semantic, LOOP3){EXPECT_NO_THROW(runSemantic("loop3.in"));}
TEST(Semantic, LOOP4){EXPECT_ANY_THROW(runSemantic("loop4.in"));}
TEST(Semantic, LOOP5){EXPECT_ANY_THROW(runSemantic("loop5.in"));}
TEST(Semantic, LOOP6){EXPECT_ANY_THROW(runSemantic("loop6.in"));}
TEST(Semantic, TYPE1){EXPECT_NO_THROW(runSemantic("type1.in"));}
TEST(Semantic, TYPE2){EXPECT_NO_THROW(runSemantic("type2.in"));}
TEST(Semantic, EXPR1){EXPECT_NO_THROW(runSemantic("expr1.in"));}
TEST(Semantic, EXPR2){EXPECT_ANY_THROW(runSemantic("expr2.in"));}
TEST(Semantic, EXPR3){EXPECT_NO_THROW(runSemantic("expr3.in"));}

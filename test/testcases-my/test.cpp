#include "../../src/parser.h"
#include "../../src/type.h"
#include<iostream>
#include <gtest/gtest.h>

std::string openFile(std::string path)
{
    path="../../../test/testcases-my/testcases/"+path;
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
TEST(Parser, IF2){EXPECT_NO_THROW(runParser("if2.in"));}
TEST(Parser, IF3){EXPECT_NO_THROW(runParser("if3.in"));}
TEST(Parser, LOOP1){EXPECT_NO_THROW(runParser("loop1.in"));}
TEST(Parser, LOOP2){EXPECT_NO_THROW(runParser("loop2.in"));}
TEST(Parser, LOOP3){EXPECT_NO_THROW(runParser("loop3.in"));}
TEST(Parser, LOOP4){EXPECT_NO_THROW(runParser("loop4.in"));}
TEST(Parser, LOOP5){EXPECT_NO_THROW(runParser("loop5.in"));}
TEST(Parser, LOOP6){EXPECT_NO_THROW(runParser("loop6.in"));}
TEST(Parser, TYPE1){EXPECT_NO_THROW(runParser("type1.in"));}
TEST(Parser, TYPE2){EXPECT_NO_THROW(runParser("type2.in"));}
TEST(Parser, TYPE3){EXPECT_NO_THROW(runParser("type3.in"));}
TEST(Parser, TYPE4){EXPECT_NO_THROW(runParser("type4.in"));}
TEST(Parser, TYPE5){EXPECT_NO_THROW(runParser("type5.in"));}
TEST(Parser, TYPE6){EXPECT_NO_THROW(runParser("type6.in"));}
TEST(Parser, TYPE7){EXPECT_NO_THROW(runParser("type7.in"));}
TEST(Parser, TYPE8){EXPECT_NO_THROW(runParser("type8.in"));}
TEST(Parser, EXPR1){EXPECT_NO_THROW(runParser("expr1.in"));}
TEST(Parser, EXPR2){EXPECT_ANY_THROW(runParser("expr2.in"));}
TEST(Parser, EXPR3){EXPECT_NO_THROW(runParser("expr3.in"));}
TEST(Parser, ARRAY1){EXPECT_NO_THROW(runParser("array1.in"));}
TEST(Parser, ARRAY2){EXPECT_NO_THROW(runParser("array2.in"));}
TEST(Parser, REF1){EXPECT_NO_THROW(runParser("ref1.in"));}
TEST(Parser, REF2){EXPECT_NO_THROW(runParser("ref2.in"));}
TEST(Parser, REF3){EXPECT_NO_THROW(runParser("ref3.in"));}
TEST(Parser, REF4){EXPECT_NO_THROW(runParser("ref4.in"));}
TEST(Parser, REF5){EXPECT_NO_THROW(runParser("ref5.in"));}
TEST(Parser, REF6){EXPECT_NO_THROW(runParser("ref6.in"));}
TEST(Parser, REF7){EXPECT_NO_THROW(runParser("ref7.in"));}
TEST(Parser, REF8){EXPECT_NO_THROW(runParser("ref8.in"));}

TEST(Semantic, IF1){EXPECT_NO_THROW(runSemantic("if1.in"));}
TEST(Semantic, IF2){EXPECT_NO_THROW(runSemantic("if2.in"));}
TEST(Semantic, IF3){EXPECT_ANY_THROW(runSemantic("if3.in"));}
TEST(Semantic, LOOP1){EXPECT_ANY_THROW(runSemantic("loop1.in"));}
TEST(Semantic, LOOP2){EXPECT_NO_THROW(runSemantic("loop2.in"));}
TEST(Semantic, LOOP3){EXPECT_NO_THROW(runSemantic("loop3.in"));}
TEST(Semantic, LOOP4){EXPECT_ANY_THROW(runSemantic("loop4.in"));}
TEST(Semantic, LOOP5){EXPECT_ANY_THROW(runSemantic("loop5.in"));}
TEST(Semantic, LOOP6){EXPECT_ANY_THROW(runSemantic("loop6.in"));}
TEST(Semantic, TYPE1){EXPECT_NO_THROW(runSemantic("type1.in"));}
TEST(Semantic, TYPE2){EXPECT_NO_THROW(runSemantic("type2.in"));}
TEST(Semantic, TYPE3){EXPECT_NO_THROW(runSemantic("type3.in"));}
TEST(Semantic, TYPE4){EXPECT_ANY_THROW(runSemantic("type4.in"));}
TEST(Semantic, TYPE5){EXPECT_NO_THROW(runSemantic("type5.in"));}
TEST(Semantic, TYPE6){EXPECT_NO_THROW(runSemantic("type6.in"));}
TEST(Semantic, TYPE7){EXPECT_ANY_THROW(runSemantic("type7.in"));}
TEST(Semantic, TYPE8){EXPECT_NO_THROW(runSemantic("type8.in"));}
TEST(Semantic, EXPR1){EXPECT_NO_THROW(runSemantic("expr1.in"));}
TEST(Semantic, EXPR2){EXPECT_ANY_THROW(runSemantic("expr2.in"));}
TEST(Semantic, EXPR3){EXPECT_NO_THROW(runSemantic("expr3.in"));}
TEST(Semantic, ARRAY1){EXPECT_NO_THROW(runSemantic("array1.in"));}
TEST(Semantic, ARRAY2){EXPECT_NO_THROW(runSemantic("array2.in"));}
TEST(Semantic, REF1){EXPECT_NO_THROW(runSemantic("ref1.in"));}
TEST(Semantic, REF2){EXPECT_NO_THROW(runSemantic("ref2.in"));}
TEST(Semantic, REF3){EXPECT_ANY_THROW(runSemantic("ref3.in"));}
TEST(Semantic, REF4){EXPECT_ANY_THROW(runSemantic("ref4.in"));}
TEST(Semantic, REF5){EXPECT_ANY_THROW(runSemantic("ref5.in"));}
TEST(Semantic, REF6){EXPECT_ANY_THROW(runSemantic("ref6.in"));}
TEST(Semantic, REF7){EXPECT_ANY_THROW(runSemantic("ref7.in"));}
TEST(Semantic, REF8){EXPECT_ANY_THROW(runSemantic("ref8.in"));}

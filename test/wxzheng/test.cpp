#include "../../src/parser.h"
#include "../../src/semantic.h"
#include "../../src/ir.h"
#include<iostream>
#include<fstream>
#include<gtest/gtest.h>
#include<chrono>

std::string openFile(std::string path)
{
    path = path+".rx";
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

void runSemantic(std::string path)
{
    const auto code=openFile(path);
    semanticCheck(parser(code).solve());
}

void runIr(std::string path)
{
    const auto code=openFile(path);
    auto las=std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()).time_since_epoch().count();
    auto node=parser(code).solve();
    semanticCheck(node);
    std::fstream fs("my.ll", std::ios::out);
    auto irCode=generateIr(node);
    for (const auto& t:irCode)
        fs<<t<<std::endl;
    fs<<std::endl;
    auto now=std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()).time_since_epoch().count();
    std::cerr<<"my time: "<<now-las<<" ms"<<std::endl;
    las=now;
    auto cl=system("clang --target=x86_64-unknown-linux-gnu -march=x86-64 -O0 my.ll -o my");
    now=std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()).time_since_epoch().count();
    std::cerr<<"clang time: "<<now-las<<" ms"<<std::endl;
    if (cl)
        throw compileError();
    las=now;
    cl=system(("./my > my.out < "+path+".in").c_str());
    now=std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()).time_since_epoch().count();
    std::cerr<<"run time: "<<now-las<<" ms"<<std::endl;
    if (WIFSIGNALED(cl))
    {
        std::cerr<<"Runtime Error"<<std::endl;
        throw compileError();
    }
    cl=system(("diff my.out "+path+".out -w").c_str());
    if (cl)
    {
        std::cerr<<"Wrong Answer"<<std::endl;
        throw compileError();
    }
}

TEST(semantic2, comprehensive1) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive1"));
}
TEST(IR, comprehensive1) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive1"));
}
TEST(semantic2, comprehensive10) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive10"));
}
TEST(IR, comprehensive10) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive10"));
}
TEST(semantic2, comprehensive11) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive11"));
}
TEST(IR, comprehensive11) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive11"));
}
TEST(semantic2, comprehensive12) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive12"));
}
TEST(IR, comprehensive12) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive12"));
}
TEST(semantic2, comprehensive13) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive13"));
}
TEST(IR, comprehensive13) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive13"));
}
TEST(semantic2, comprehensive14) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive14"));
}
TEST(IR, comprehensive14) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive14"));
}
TEST(semantic2, comprehensive15) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive15"));
}
TEST(IR, comprehensive15) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive15"));
}
TEST(semantic2, comprehensive16) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive16"));
}
TEST(IR, comprehensive16) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive16"));
}
TEST(semantic2, comprehensive17) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive17"));
}
TEST(IR, comprehensive17) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive17"));
}
TEST(semantic2, comprehensive18) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive18"));
}
TEST(IR, comprehensive18) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive18"));
}
TEST(semantic2, comprehensive19) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive19"));
}
TEST(IR, comprehensive19) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive19"));
}
TEST(semantic2, comprehensive2) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive2"));
}
TEST(IR, comprehensive2) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive2"));
}
TEST(semantic2, comprehensive20) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive20"));
}
TEST(IR, comprehensive20) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive20"));
}
TEST(semantic2, comprehensive21) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive21"));
}
TEST(IR, comprehensive21) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive21"));
}
TEST(semantic2, comprehensive22) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive22"));
}
TEST(IR, comprehensive22) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive22"));
}
TEST(semantic2, comprehensive23) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive23"));
}
TEST(IR, comprehensive23) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive23"));
}
TEST(semantic2, comprehensive24) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive24"));
}
TEST(IR, comprehensive24) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive24"));
}
TEST(semantic2, comprehensive25) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive25"));
}
TEST(IR, comprehensive25) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive25"));
}
TEST(semantic2, comprehensive26) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive26"));
}
TEST(IR, comprehensive26) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive26"));
}
TEST(semantic2, comprehensive27) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive27"));
}
TEST(IR, comprehensive27) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive27"));
}
TEST(semantic2, comprehensive28) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive28"));
}
TEST(IR, comprehensive28) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive28"));
}
TEST(semantic2, comprehensive29) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive29"));
}
TEST(IR, comprehensive29) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive29"));
}
TEST(semantic2, comprehensive3) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive3"));
}
TEST(IR, comprehensive3) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive3"));
}
TEST(semantic2, comprehensive30) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive30"));
}
TEST(IR, comprehensive30) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive30"));
}
TEST(semantic2, comprehensive31) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive31"));
}
TEST(IR, comprehensive31) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive31"));
}
TEST(semantic2, comprehensive32) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive32"));
}
TEST(IR, comprehensive32) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive32"));
}
TEST(semantic2, comprehensive33) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive33"));
}
TEST(IR, comprehensive33) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive33"));
}
TEST(semantic2, comprehensive34) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive34"));
}
TEST(IR, comprehensive34) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive34"));
}
TEST(semantic2, comprehensive35) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive35"));
}
TEST(IR, comprehensive35) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive35"));
}
TEST(semantic2, comprehensive36) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive36"));
}
TEST(IR, comprehensive36) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive36"));
}
TEST(semantic2, comprehensive37) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive37"));
}
TEST(IR, comprehensive37) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive37"));
}
TEST(semantic2, comprehensive38) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive38"));
}
TEST(IR, comprehensive38) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive38"));
}
TEST(semantic2, comprehensive39) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive39"));
}
TEST(IR, comprehensive39) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive39"));
}
TEST(semantic2, comprehensive4) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive4"));
}
TEST(IR, comprehensive4) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive4"));
}
TEST(semantic2, comprehensive40) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive40"));
}
TEST(IR, comprehensive40) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive40"));
}
TEST(semantic2, comprehensive41) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive41"));
}
TEST(IR, comprehensive41) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive41"));
}
TEST(semantic2, comprehensive42) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive42"));
}
TEST(IR, comprehensive42) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive42"));
}
TEST(semantic2, comprehensive43) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive43"));
}
TEST(IR, comprehensive43) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive43"));
}
TEST(semantic2, comprehensive44) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive44"));
}
TEST(IR, comprehensive44) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive44"));
}
TEST(semantic2, comprehensive45) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive45"));
}
TEST(IR, comprehensive45) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive45"));
}
TEST(semantic2, comprehensive46) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive46"));
}
TEST(IR, comprehensive46) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive46"));
}
TEST(semantic2, comprehensive47) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive47"));
}
TEST(IR, comprehensive47) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive47"));
}
TEST(semantic2, comprehensive48) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive48"));
}
TEST(IR, comprehensive48) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive48"));
}
TEST(semantic2, comprehensive49) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive49"));
}
TEST(IR, comprehensive49) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive49"));
}
TEST(semantic2, comprehensive5) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive5"));
}
TEST(IR, comprehensive5) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive5"));
}
TEST(semantic2, comprehensive50) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive50"));
}
TEST(IR, comprehensive50) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive50"));
}
TEST(semantic2, comprehensive6) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive6"));
}
TEST(IR, comprehensive6) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive6"));
}
TEST(semantic2, comprehensive7) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive7"));
}
TEST(IR, comprehensive7) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive7"));
}
TEST(semantic2, comprehensive8) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive8"));
}
TEST(IR, comprehensive8) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive8"));
}
TEST(semantic2, comprehensive9) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/comprehensive9"));
}
TEST(IR, comprehensive9) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/comprehensive9"));
}
TEST(semantic1, array1) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/array1"));
}
TEST(IR, array1) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/array1"));
}
TEST(semantic1, array2) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/array2"));
}
TEST(IR, array2) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/array2"));
}
TEST(semantic1, array3) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/array3"));
}
TEST(IR, array3) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/array3"));
}
TEST(semantic1, array4) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/array4"));
}
TEST(semantic1, array5) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/array5"));
}
TEST(semantic1, array6) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/array6"));
}
TEST(semantic1, array7) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/array7"));
}
TEST(semantic1, array8) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/array8"));
}
TEST(semantic1, autoref1) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/autoref1"));
}
TEST(IR, autoref1) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/autoref1"));
}
TEST(semantic1, autoref2) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/autoref2"));
}
TEST(IR, autoref2) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/autoref2"));
}
TEST(semantic1, autoref3) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/autoref3"));
}
TEST(IR, autoref3) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/autoref3"));
}
TEST(semantic1, autoref4) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/autoref4"));
}
TEST(IR, autoref4) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/autoref4"));
}
TEST(semantic1, autoref5) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/autoref5"));
}
TEST(IR, autoref5) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/autoref5"));
}
TEST(semantic1, autoref6) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/autoref6"));
}
TEST(IR, autoref6) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/autoref6"));
}
TEST(semantic1, autoref7) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/autoref7"));
}
TEST(semantic1, autoref8) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/autoref8"));
}
TEST(semantic1, autoref9) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/autoref9"));
}
TEST(IR, autoref9) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/autoref9"));
}
TEST(semantic1, basic1) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic1"));
}
TEST(IR, basic1) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic1"));
}
TEST(semantic1, basic10) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic10"));
}
TEST(semantic1, basic11) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic11"));
}
TEST(IR, basic11) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic11"));
}
TEST(semantic1, basic12) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic12"));
}
TEST(semantic1, basic13) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic13"));
}
TEST(semantic1, basic14) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic14"));
}
TEST(semantic1, basic15) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic15"));
}
TEST(semantic1, basic16) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic16"));
}
TEST(semantic1, basic17) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic17"));
}
TEST(IR, basic17) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic17"));
}
TEST(semantic1, basic18) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic18"));
}
TEST(IR, basic18) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic18"));
}
TEST(semantic1, basic19) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic19"));
}
TEST(IR, basic19) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic19"));
}
TEST(semantic1, basic2) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic2"));
}
TEST(semantic1, basic20) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic20"));
}
TEST(IR, basic20) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic20"));
}
TEST(semantic1, basic21) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic21"));
}
TEST(IR, basic21) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic21"));
}
TEST(semantic1, basic22) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic22"));
}
TEST(IR, basic22) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic22"));
}
TEST(semantic1, basic23) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic23"));
}
TEST(IR, basic23) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic23"));
}
TEST(semantic1, basic24) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic24"));
}
TEST(IR, basic24) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic24"));
}
TEST(semantic1, basic25) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic25"));
}
TEST(IR, basic25) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic25"));
}
TEST(semantic1, basic26) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic26"));
}
TEST(IR, basic26) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic26"));
}
TEST(semantic1, basic27) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic27"));
}
TEST(IR, basic27) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic27"));
}
TEST(semantic1, basic28) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic28"));
}
TEST(semantic1, basic29) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic29"));
}
TEST(IR, basic29) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic29"));
}
TEST(semantic1, basic3) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic3"));
}
TEST(semantic1, basic30) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic30"));
}
TEST(semantic1, basic31) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic31"));
}
TEST(semantic1, basic32) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic32"));
}
TEST(semantic1, basic33) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic33"));
}
TEST(semantic1, basic34) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic34"));
}
TEST(semantic1, basic35) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic35"));
}
TEST(semantic1, basic36) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic36"));
}
TEST(IR, basic36) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic36"));
}
TEST(semantic1, basic37) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic37"));
}
TEST(IR, basic37) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic37"));
}
TEST(semantic1, basic38) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic38"));
}
TEST(semantic1, basic39) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic39"));
}
TEST(IR, basic39) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic39"));
}
TEST(semantic1, basic4) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic4"));
}
TEST(IR, basic4) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic4"));
}
TEST(semantic1, basic40) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/basic40"));
}
TEST(IR, basic40) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/basic40"));
}
TEST(semantic1, basic5) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic5"));
}
TEST(semantic1, basic6) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic6"));
}
TEST(semantic1, basic7) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic7"));
}
TEST(semantic1, basic8) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic8"));
}
TEST(semantic1, basic9) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/basic9"));
}
TEST(semantic1, expr1) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr1"));
}
TEST(semantic1, expr10) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr10"));
}
TEST(semantic1, expr11) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr11"));
}
TEST(semantic1, expr12) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr12"));
}
TEST(semantic1, expr13) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/expr13"));
}
TEST(IR, expr13) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/expr13"));
}
TEST(semantic1, expr14) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr14"));
}
TEST(semantic1, expr15) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr15"));
}
TEST(semantic1, expr16) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr16"));
}
TEST(semantic1, expr17) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/expr17"));
}
TEST(IR, expr17) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/expr17"));
}
TEST(semantic1, expr18) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr18"));
}
TEST(semantic1, expr19) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/expr19"));
}
TEST(IR, expr19) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/expr19"));
}
TEST(semantic1, expr2) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr2"));
}
TEST(semantic1, expr20) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr20"));
}
TEST(semantic1, expr21) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr21"));
}
TEST(semantic1, expr22) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr22"));
}
TEST(semantic1, expr23) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr23"));
}
TEST(semantic1, expr24) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr24"));
}
TEST(semantic1, expr25) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr25"));
}
TEST(semantic1, expr26) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr26"));
}
TEST(semantic1, expr27) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr27"));
}
TEST(semantic1, expr28) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr28"));
}
TEST(semantic1, expr29) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr29"));
}
TEST(semantic1, expr3) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr3"));
}
TEST(semantic1, expr30) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr30"));
}
TEST(semantic1, expr31) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr31"));
}
TEST(semantic1, expr32) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr32"));
}
TEST(semantic1, expr33) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/expr33"));
}
TEST(IR, expr33) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/expr33"));
}
TEST(semantic1, expr34) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/expr34"));
}
TEST(IR, expr34) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/expr34"));
}
TEST(semantic1, expr35) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr35"));
}
TEST(semantic1, expr36) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/expr36"));
}
TEST(IR, expr36) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/expr36"));
}
TEST(semantic1, expr37) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr37"));
}
TEST(semantic1, expr38) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/expr38"));
}
TEST(IR, expr38) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/expr38"));
}
TEST(semantic1, expr39) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr39"));
}
TEST(semantic1, expr4) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr4"));
}
TEST(semantic1, expr40) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr40"));
}
TEST(semantic1, expr5) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr5"));
}
TEST(semantic1, expr6) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr6"));
}
TEST(semantic1, expr7) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr7"));
}
TEST(semantic1, expr8) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr8"));
}
TEST(semantic1, expr9) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/expr9"));
}
TEST(semantic1, if1) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/if1"));
}
TEST(IR, if1) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/if1"));
}
TEST(semantic1, if10) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/if10"));
}
TEST(IR, if10) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/if10"));
}
TEST(semantic1, if11) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/if11"));
}
TEST(semantic1, if12) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/if12"));
}
TEST(semantic1, if13) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/if13"));
}
TEST(semantic1, if14) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/if14"));
}
TEST(semantic1, if15) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/if15"));
}
TEST(semantic1, if2) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/if2"));
}
TEST(IR, if2) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/if2"));
}
TEST(semantic1, if3) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/if3"));
}
TEST(IR, if3) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/if3"));
}
TEST(semantic1, if4) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/if4"));
}
TEST(IR, if4) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/if4"));
}
TEST(semantic1, if5) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/if5"));
}
TEST(IR, if5) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/if5"));
}
TEST(semantic1, if6) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/if6"));
}
TEST(IR, if6) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/if6"));
}
TEST(semantic1, if7) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/if7"));
}
TEST(IR, if7) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/if7"));
}
TEST(semantic1, if8) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/if8"));
}
TEST(IR, if8) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/if8"));
}
TEST(semantic1, if9) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/if9"));
}
TEST(IR, if9) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/if9"));
}
TEST(semantic1, loop1) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/loop1"));
}
TEST(IR, loop1) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/loop1"));
}
TEST(semantic1, loop10) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/loop10"));
}
TEST(semantic1, loop2) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/loop2"));
}
TEST(IR, loop2) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/loop2"));
}
TEST(semantic1, loop3) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/loop3"));
}
TEST(IR, loop3) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/loop3"));
}
TEST(semantic1, loop4) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/loop4"));
}
TEST(IR, loop4) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/loop4"));
}
TEST(semantic1, loop5) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/loop5"));
}
TEST(IR, loop5) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/loop5"));
}
TEST(semantic1, loop6) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/loop6"));
}
TEST(semantic1, loop7) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/loop7"));
}
TEST(semantic1, loop8) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/loop8"));
}
TEST(semantic1, loop9) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/loop9"));
}
TEST(semantic1, misc1) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc1"));
}
TEST(IR, misc1) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc1"));
}
TEST(semantic1, misc10) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc10"));
}
TEST(IR, misc10) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc10"));
}
TEST(semantic1, misc11) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc11"));
}
TEST(IR, misc11) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc11"));
}
TEST(semantic1, misc12) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc12"));
}
TEST(IR, misc12) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc12"));
}
TEST(semantic1, misc13) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc13"));
}
TEST(IR, misc13) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc13"));
}
TEST(semantic1, misc14) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc14"));
}
TEST(IR, misc14) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc14"));
}
TEST(semantic1, misc15) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc15"));
}
TEST(IR, misc15) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc15"));
}
TEST(semantic1, misc16) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc16"));
}
TEST(IR, misc16) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc16"));
}
TEST(semantic1, misc17) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc17"));
}
TEST(IR, misc17) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc17"));
}
TEST(semantic1, misc18) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc18"));
}
TEST(IR, misc18) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc18"));
}
TEST(semantic1, misc19) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc19"));
}
TEST(IR, misc19) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc19"));
}
TEST(semantic1, misc2) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc2"));
}
TEST(IR, misc2) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc2"));
}
TEST(semantic1, misc20) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc20"));
}
TEST(IR, misc20) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc20"));
}
TEST(semantic1, misc21) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc21"));
}
TEST(IR, misc21) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc21"));
}
TEST(semantic1, misc22) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc22"));
}
TEST(IR, misc22) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc22"));
}
TEST(semantic1, misc23) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc23"));
}
TEST(IR, misc23) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc23"));
}
TEST(semantic1, misc24) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc24"));
}
TEST(IR, misc24) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc24"));
}
TEST(semantic1, misc25) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc25"));
}
TEST(IR, misc25) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc25"));
}
TEST(semantic1, misc26) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc26"));
}
TEST(IR, misc26) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc26"));
}
TEST(semantic1, misc27) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc27"));
}
TEST(IR, misc27) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc27"));
}
TEST(semantic1, misc28) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc28"));
}
TEST(IR, misc28) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc28"));
}
TEST(semantic1, misc29) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc29"));
}
TEST(IR, misc29) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc29"));
}
TEST(semantic1, misc3) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc3"));
}
TEST(IR, misc3) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc3"));
}
TEST(semantic1, misc30) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc30"));
}
TEST(IR, misc30) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc30"));
}
TEST(semantic1, misc31) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc31"));
}
TEST(IR, misc31) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc31"));
}
TEST(semantic1, misc32) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc32"));
}
TEST(IR, misc32) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc32"));
}
TEST(semantic1, misc33) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc33"));
}
TEST(IR, misc33) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc33"));
}
TEST(semantic1, misc34) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc34"));
}
TEST(IR, misc34) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc34"));
}
TEST(semantic1, misc35) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc35"));
}
TEST(IR, misc35) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc35"));
}
TEST(semantic1, misc36) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc36"));
}
TEST(IR, misc36) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc36"));
}
TEST(semantic1, misc37) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc37"));
}
TEST(IR, misc37) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc37"));
}
TEST(semantic1, misc38) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc38"));
}
TEST(IR, misc38) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc38"));
}
TEST(semantic1, misc39) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc39"));
}
TEST(IR, misc39) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc39"));
}
TEST(semantic1, misc4) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc4"));
}
TEST(IR, misc4) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc4"));
}
TEST(semantic1, misc40) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc40"));
}
TEST(IR, misc40) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc40"));
}
TEST(semantic1, misc41) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc41"));
}
TEST(IR, misc41) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc41"));
}
TEST(semantic1, misc42) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc42"));
}
TEST(IR, misc42) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc42"));
}
TEST(semantic1, misc43) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc43"));
}
TEST(IR, misc43) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc43"));
}
TEST(semantic1, misc44) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc44"));
}
TEST(IR, misc44) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc44"));
}
TEST(semantic1, misc45) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc45"));
}
TEST(IR, misc45) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc45"));
}
TEST(semantic1, misc46) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc46"));
}
TEST(IR, misc46) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc46"));
}
TEST(semantic1, misc47) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc47"));
}
TEST(IR, misc47) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc47"));
}
TEST(semantic1, misc48) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc48"));
}
TEST(IR, misc48) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc48"));
}
TEST(semantic1, misc49) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc49"));
}
TEST(IR, misc49) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc49"));
}
TEST(semantic1, misc5) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc5"));
}
TEST(IR, misc5) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc5"));
}
TEST(semantic1, misc50) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc50"));
}
TEST(IR, misc50) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc50"));
}
TEST(semantic1, misc51) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc51"));
}
TEST(IR, misc51) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc51"));
}
TEST(semantic1, misc52) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc52"));
}
TEST(IR, misc52) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc52"));
}
TEST(semantic1, misc53) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc53"));
}
TEST(IR, misc53) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc53"));
}
TEST(semantic1, misc54) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc54"));
}
TEST(IR, misc54) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc54"));
}
TEST(semantic1, misc55) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc55"));
}
TEST(IR, misc55) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc55"));
}
TEST(semantic1, misc56) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/misc56"));
}
TEST(semantic1, misc57) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/misc57"));
}
TEST(semantic1, misc58) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/misc58"));
}
TEST(semantic1, misc59) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/misc59"));
}
TEST(semantic1, misc6) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc6"));
}
TEST(IR, misc6) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc6"));
}
TEST(semantic1, misc60) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/misc60"));
}
TEST(semantic1, misc61) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/misc61"));
}
TEST(semantic1, misc62) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/misc62"));
}
TEST(semantic1, misc63) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/misc63"));
}
TEST(semantic1, misc64) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/misc64"));
}
TEST(semantic1, misc65) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc65"));
}
TEST(IR, misc65) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc65"));
}
TEST(semantic1, misc7) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc7"));
}
TEST(IR, misc7) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc7"));
}
TEST(semantic1, misc8) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc8"));
}
TEST(IR, misc8) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc8"));
}
TEST(semantic1, misc9) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/misc9"));
}
TEST(IR, misc9) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/misc9"));
}
TEST(semantic1, return1) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/return1"));
}
TEST(semantic1, return10) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/return10"));
}
TEST(semantic1, return11) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/return11"));
}
TEST(semantic1, return12) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/return12"));
}
TEST(IR, return12) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/return12"));
}
TEST(semantic1, return13) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/return13"));
}
TEST(IR, return13) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/return13"));
}
TEST(semantic1, return14) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/return14"));
}
TEST(IR, return14) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/return14"));
}
TEST(semantic1, return15) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/return15"));
}
TEST(IR, return15) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/return15"));
}
TEST(semantic1, return2) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/return2"));
}
TEST(IR, return2) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/return2"));
}
TEST(semantic1, return3) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/return3"));
}
TEST(semantic1, return4) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/return4"));
}
TEST(semantic1, return5) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/return5"));
}
TEST(semantic1, return6) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/return6"));
}
TEST(semantic1, return7) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/return7"));
}
TEST(semantic1, return8) {
    EXPECT_NO_THROW(runSemantic("../../../test/wxzheng/testcases/return8"));
}
TEST(IR, return8) {
    EXPECT_NO_THROW(runIr("../../../test/wxzheng/testcases/return8"));
}
TEST(semantic1, return9) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/return9"));
}
TEST(semantic1, type1) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type1"));
}
TEST(semantic1, type10) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type10"));
}
TEST(semantic1, type11) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type11"));
}
TEST(semantic1, type12) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type12"));
}
TEST(semantic1, type13) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type13"));
}
TEST(semantic1, type14) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type14"));
}
TEST(semantic1, type15) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type15"));
}
TEST(semantic1, type16) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type16"));
}
TEST(semantic1, type17) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type17"));
}
TEST(semantic1, type18) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type18"));
}
TEST(semantic1, type19) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type19"));
}
TEST(semantic1, type2) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type2"));
}
TEST(semantic1, type20) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type20"));
}
TEST(semantic1, type3) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type3"));
}
TEST(semantic1, type4) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type4"));
}
TEST(semantic1, type5) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type5"));
}
TEST(semantic1, type6) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type6"));
}
TEST(semantic1, type7) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type7"));
}
TEST(semantic1, type8) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type8"));
}
TEST(semantic1, type9) {
    EXPECT_ANY_THROW(runSemantic("../../../test/wxzheng/testcases/type9"));
}
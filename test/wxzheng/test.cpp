#include "../../src/parser.h"
#include "../../src/semantic.h"
#include "../../src/ir.h"
#include<iostream>
#include<fstream>
#include<gtest/gtest.h>
#include<chrono>

std::string openFile(std::string path)
{
    path="../../../test/wxzheng/testcases/"+path;
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
    auto cl=system("clang -S --target=riscv32-unknown-elf -march=rv32gc -mabi=ilp32d"\
                   " -O0 my.ll -o my.s");
    now=std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()).time_since_epoch().count();
    std::cerr<<"clang time: "<<now-las<<" ms"<<std::endl;
    if (cl)
        throw compileError();
}

TEST(semantic2, comprehensive1) {
    EXPECT_NO_THROW(runSemantic("comprehensive1.in"));
}
TEST(IR, comprehensive1) {
    EXPECT_NO_THROW(runIr("comprehensive1.in"));
}
TEST(semantic2, comprehensive10) {
    EXPECT_NO_THROW(runSemantic("comprehensive10.in"));
}
TEST(IR, comprehensive10) {
    EXPECT_NO_THROW(runIr("comprehensive10.in"));
}
TEST(semantic2, comprehensive11) {
    EXPECT_NO_THROW(runSemantic("comprehensive11.in"));
}
TEST(IR, comprehensive11) {
    EXPECT_NO_THROW(runIr("comprehensive11.in"));
}
TEST(semantic2, comprehensive12) {
    EXPECT_NO_THROW(runSemantic("comprehensive12.in"));
}
TEST(IR, comprehensive12) {
    EXPECT_NO_THROW(runIr("comprehensive12.in"));
}
TEST(semantic2, comprehensive13) {
    EXPECT_NO_THROW(runSemantic("comprehensive13.in"));
}
TEST(IR, comprehensive13) {
    EXPECT_NO_THROW(runIr("comprehensive13.in"));
}
TEST(semantic2, comprehensive14) {
    EXPECT_NO_THROW(runSemantic("comprehensive14.in"));
}
TEST(IR, comprehensive14) {
    EXPECT_NO_THROW(runIr("comprehensive14.in"));
}
TEST(semantic2, comprehensive15) {
    EXPECT_NO_THROW(runSemantic("comprehensive15.in"));
}
TEST(IR, comprehensive15) {
    EXPECT_NO_THROW(runIr("comprehensive15.in"));
}
TEST(semantic2, comprehensive16) {
    EXPECT_NO_THROW(runSemantic("comprehensive16.in"));
}
TEST(IR, comprehensive16) {
    EXPECT_NO_THROW(runIr("comprehensive16.in"));
}
TEST(semantic2, comprehensive17) {
    EXPECT_NO_THROW(runSemantic("comprehensive17.in"));
}
TEST(IR, comprehensive17) {
    EXPECT_NO_THROW(runIr("comprehensive17.in"));
}
TEST(semantic2, comprehensive18) {
    EXPECT_NO_THROW(runSemantic("comprehensive18.in"));
}
TEST(IR, comprehensive18) {
    EXPECT_NO_THROW(runIr("comprehensive18.in"));
}
TEST(semantic2, comprehensive19) {
    EXPECT_NO_THROW(runSemantic("comprehensive19.in"));
}
TEST(IR, comprehensive19) {
    EXPECT_NO_THROW(runIr("comprehensive19.in"));
}
TEST(semantic2, comprehensive2) {
    EXPECT_NO_THROW(runSemantic("comprehensive2.in"));
}
TEST(IR, comprehensive2) {
    EXPECT_NO_THROW(runIr("comprehensive2.in"));
}
TEST(semantic2, comprehensive20) {
    EXPECT_NO_THROW(runSemantic("comprehensive20.in"));
}
TEST(IR, comprehensive20) {
    EXPECT_NO_THROW(runIr("comprehensive20.in"));
}
TEST(semantic2, comprehensive21) {
    EXPECT_NO_THROW(runSemantic("comprehensive21.in"));
}
TEST(IR, comprehensive21) {
    EXPECT_NO_THROW(runIr("comprehensive21.in"));
}
TEST(semantic2, comprehensive22) {
    EXPECT_NO_THROW(runSemantic("comprehensive22.in"));
}
TEST(IR, comprehensive22) {
    EXPECT_NO_THROW(runIr("comprehensive22.in"));
}
TEST(semantic2, comprehensive23) {
    EXPECT_NO_THROW(runSemantic("comprehensive23.in"));
}
TEST(IR, comprehensive23) {
    EXPECT_NO_THROW(runIr("comprehensive23.in"));
}
TEST(semantic2, comprehensive24) {
    EXPECT_NO_THROW(runSemantic("comprehensive24.in"));
}
TEST(IR, comprehensive24) {
    EXPECT_NO_THROW(runIr("comprehensive24.in"));
}
TEST(semantic2, comprehensive25) {
    EXPECT_NO_THROW(runSemantic("comprehensive25.in"));
}
TEST(IR, comprehensive25) {
    EXPECT_NO_THROW(runIr("comprehensive25.in"));
}
TEST(semantic2, comprehensive26) {
    EXPECT_NO_THROW(runSemantic("comprehensive26.in"));
}
TEST(IR, comprehensive26) {
    EXPECT_NO_THROW(runIr("comprehensive26.in"));
}
TEST(semantic2, comprehensive27) {
    EXPECT_NO_THROW(runSemantic("comprehensive27.in"));
}
TEST(IR, comprehensive27) {
    EXPECT_NO_THROW(runIr("comprehensive27.in"));
}
TEST(semantic2, comprehensive28) {
    EXPECT_NO_THROW(runSemantic("comprehensive28.in"));
}
TEST(IR, comprehensive28) {
    EXPECT_NO_THROW(runIr("comprehensive28.in"));
}
TEST(semantic2, comprehensive29) {
    EXPECT_NO_THROW(runSemantic("comprehensive29.in"));
}
TEST(IR, comprehensive29) {
    EXPECT_NO_THROW(runIr("comprehensive29.in"));
}
TEST(semantic2, comprehensive3) {
    EXPECT_NO_THROW(runSemantic("comprehensive3.in"));
}
TEST(IR, comprehensive3) {
    EXPECT_NO_THROW(runIr("comprehensive3.in"));
}
TEST(semantic2, comprehensive30) {
    EXPECT_NO_THROW(runSemantic("comprehensive30.in"));
}
TEST(IR, comprehensive30) {
    EXPECT_NO_THROW(runIr("comprehensive30.in"));
}
TEST(semantic2, comprehensive31) {
    EXPECT_NO_THROW(runSemantic("comprehensive31.in"));
}
TEST(IR, comprehensive31) {
    EXPECT_NO_THROW(runIr("comprehensive31.in"));
}
TEST(semantic2, comprehensive32) {
    EXPECT_NO_THROW(runSemantic("comprehensive32.in"));
}
TEST(IR, comprehensive32) {
    EXPECT_NO_THROW(runIr("comprehensive32.in"));
}
TEST(semantic2, comprehensive33) {
    EXPECT_NO_THROW(runSemantic("comprehensive33.in"));
}
TEST(IR, comprehensive33) {
    EXPECT_NO_THROW(runIr("comprehensive33.in"));
}
TEST(semantic2, comprehensive34) {
    EXPECT_NO_THROW(runSemantic("comprehensive34.in"));
}
TEST(IR, comprehensive34) {
    EXPECT_NO_THROW(runIr("comprehensive34.in"));
}
TEST(semantic2, comprehensive35) {
    EXPECT_NO_THROW(runSemantic("comprehensive35.in"));
}
TEST(IR, comprehensive35) {
    EXPECT_NO_THROW(runIr("comprehensive35.in"));
}
TEST(semantic2, comprehensive36) {
    EXPECT_NO_THROW(runSemantic("comprehensive36.in"));
}
TEST(IR, comprehensive36) {
    EXPECT_NO_THROW(runIr("comprehensive36.in"));
}
TEST(semantic2, comprehensive37) {
    EXPECT_NO_THROW(runSemantic("comprehensive37.in"));
}
TEST(IR, comprehensive37) {
    EXPECT_NO_THROW(runIr("comprehensive37.in"));
}
TEST(semantic2, comprehensive38) {
    EXPECT_NO_THROW(runSemantic("comprehensive38.in"));
}
TEST(IR, comprehensive38) {
    EXPECT_NO_THROW(runIr("comprehensive38.in"));
}
TEST(semantic2, comprehensive39) {
    EXPECT_NO_THROW(runSemantic("comprehensive39.in"));
}
TEST(IR, comprehensive39) {
    EXPECT_NO_THROW(runIr("comprehensive39.in"));
}
TEST(semantic2, comprehensive4) {
    EXPECT_NO_THROW(runSemantic("comprehensive4.in"));
}
TEST(IR, comprehensive4) {
    EXPECT_NO_THROW(runIr("comprehensive4.in"));
}
TEST(semantic2, comprehensive40) {
    EXPECT_NO_THROW(runSemantic("comprehensive40.in"));
}
TEST(IR, comprehensive40) {
    EXPECT_NO_THROW(runIr("comprehensive40.in"));
}
TEST(semantic2, comprehensive41) {
    EXPECT_NO_THROW(runSemantic("comprehensive41.in"));
}
TEST(IR, comprehensive41) {
    EXPECT_NO_THROW(runIr("comprehensive41.in"));
}
TEST(semantic2, comprehensive42) {
    EXPECT_NO_THROW(runSemantic("comprehensive42.in"));
}
TEST(IR, comprehensive42) {
    EXPECT_NO_THROW(runIr("comprehensive42.in"));
}
TEST(semantic2, comprehensive43) {
    EXPECT_NO_THROW(runSemantic("comprehensive43.in"));
}
TEST(IR, comprehensive43) {
    EXPECT_NO_THROW(runIr("comprehensive43.in"));
}
TEST(semantic2, comprehensive44) {
    EXPECT_NO_THROW(runSemantic("comprehensive44.in"));
}
TEST(IR, comprehensive44) {
    EXPECT_NO_THROW(runIr("comprehensive44.in"));
}
TEST(semantic2, comprehensive45) {
    EXPECT_NO_THROW(runSemantic("comprehensive45.in"));
}
TEST(IR, comprehensive45) {
    EXPECT_NO_THROW(runIr("comprehensive45.in"));
}
TEST(semantic2, comprehensive46) {
    EXPECT_NO_THROW(runSemantic("comprehensive46.in"));
}
TEST(IR, comprehensive46) {
    EXPECT_NO_THROW(runIr("comprehensive46.in"));
}
TEST(semantic2, comprehensive47) {
    EXPECT_NO_THROW(runSemantic("comprehensive47.in"));
}
TEST(IR, comprehensive47) {
    EXPECT_NO_THROW(runIr("comprehensive47.in"));
}
TEST(semantic2, comprehensive48) {
    EXPECT_NO_THROW(runSemantic("comprehensive48.in"));
}
TEST(IR, comprehensive48) {
    EXPECT_NO_THROW(runIr("comprehensive48.in"));
}
TEST(semantic2, comprehensive49) {
    EXPECT_NO_THROW(runSemantic("comprehensive49.in"));
}
TEST(IR, comprehensive49) {
    EXPECT_NO_THROW(runIr("comprehensive49.in"));
}
TEST(semantic2, comprehensive5) {
    EXPECT_NO_THROW(runSemantic("comprehensive5.in"));
}
TEST(IR, comprehensive5) {
    EXPECT_NO_THROW(runIr("comprehensive5.in"));
}
TEST(semantic2, comprehensive50) {
    EXPECT_NO_THROW(runSemantic("comprehensive50.in"));
}
TEST(IR, comprehensive50) {
    EXPECT_NO_THROW(runIr("comprehensive50.in"));
}
TEST(semantic2, comprehensive6) {
    EXPECT_NO_THROW(runSemantic("comprehensive6.in"));
}
TEST(IR, comprehensive6) {
    EXPECT_NO_THROW(runIr("comprehensive6.in"));
}
TEST(semantic2, comprehensive7) {
    EXPECT_NO_THROW(runSemantic("comprehensive7.in"));
}
TEST(IR, comprehensive7) {
    EXPECT_NO_THROW(runIr("comprehensive7.in"));
}
TEST(semantic2, comprehensive8) {
    EXPECT_NO_THROW(runSemantic("comprehensive8.in"));
}
TEST(IR, comprehensive8) {
    EXPECT_NO_THROW(runIr("comprehensive8.in"));
}
TEST(semantic2, comprehensive9) {
    EXPECT_NO_THROW(runSemantic("comprehensive9.in"));
}
TEST(IR, comprehensive9) {
    EXPECT_NO_THROW(runIr("comprehensive9.in"));
}
TEST(semantic1, array1) {
    EXPECT_NO_THROW(runSemantic("array1.in"));
}
TEST(IR, array1) {
    EXPECT_NO_THROW(runIr("array1.in"));
}
TEST(semantic1, array2) {
    EXPECT_NO_THROW(runSemantic("array2.in"));
}
TEST(IR, array2) {
    EXPECT_NO_THROW(runIr("array2.in"));
}
TEST(semantic1, array3) {
    EXPECT_NO_THROW(runSemantic("array3.in"));
}
TEST(IR, array3) {
    EXPECT_NO_THROW(runIr("array3.in"));
}
TEST(semantic1, array4) {
    EXPECT_ANY_THROW(runSemantic("array4.in"));
}
TEST(semantic1, array5) {
    EXPECT_ANY_THROW(runSemantic("array5.in"));
}
TEST(semantic1, array6) {
    EXPECT_ANY_THROW(runSemantic("array6.in"));
}
TEST(semantic1, array7) {
    EXPECT_ANY_THROW(runSemantic("array7.in"));
}
TEST(semantic1, array8) {
    EXPECT_ANY_THROW(runSemantic("array8.in"));
}
TEST(semantic1, autoref1) {
    EXPECT_NO_THROW(runSemantic("autoref1.in"));
}
TEST(IR, autoref1) {
    EXPECT_NO_THROW(runIr("autoref1.in"));
}
TEST(semantic1, autoref2) {
    EXPECT_NO_THROW(runSemantic("autoref2.in"));
}
TEST(IR, autoref2) {
    EXPECT_NO_THROW(runIr("autoref2.in"));
}
TEST(semantic1, autoref3) {
    EXPECT_NO_THROW(runSemantic("autoref3.in"));
}
TEST(IR, autoref3) {
    EXPECT_NO_THROW(runIr("autoref3.in"));
}
TEST(semantic1, autoref4) {
    EXPECT_NO_THROW(runSemantic("autoref4.in"));
}
TEST(IR, autoref4) {
    EXPECT_NO_THROW(runIr("autoref4.in"));
}
TEST(semantic1, autoref5) {
    EXPECT_NO_THROW(runSemantic("autoref5.in"));
}
TEST(IR, autoref5) {
    EXPECT_NO_THROW(runIr("autoref5.in"));
}
TEST(semantic1, autoref6) {
    EXPECT_NO_THROW(runSemantic("autoref6.in"));
}
TEST(IR, autoref6) {
    EXPECT_NO_THROW(runIr("autoref6.in"));
}
TEST(semantic1, autoref7) {
    EXPECT_ANY_THROW(runSemantic("autoref7.in"));
}
TEST(semantic1, autoref8) {
    EXPECT_ANY_THROW(runSemantic("autoref8.in"));
}
TEST(semantic1, autoref9) {
    EXPECT_NO_THROW(runSemantic("autoref9.in"));
}
TEST(IR, autoref9) {
    EXPECT_NO_THROW(runIr("autoref9.in"));
}
TEST(semantic1, basic1) {
    EXPECT_NO_THROW(runSemantic("basic1.in"));
}
TEST(IR, basic1) {
    EXPECT_NO_THROW(runIr("basic1.in"));
}
TEST(semantic1, basic10) {
    EXPECT_ANY_THROW(runSemantic("basic10.in"));
}
TEST(semantic1, basic11) {
    EXPECT_NO_THROW(runSemantic("basic11.in"));
}
TEST(IR, basic11) {
    EXPECT_NO_THROW(runIr("basic11.in"));
}
TEST(semantic1, basic12) {
    EXPECT_ANY_THROW(runSemantic("basic12.in"));
}
TEST(semantic1, basic13) {
    EXPECT_ANY_THROW(runSemantic("basic13.in"));
}
TEST(semantic1, basic14) {
    EXPECT_ANY_THROW(runSemantic("basic14.in"));
}
TEST(semantic1, basic15) {
    EXPECT_ANY_THROW(runSemantic("basic15.in"));
}
TEST(semantic1, basic16) {
    EXPECT_ANY_THROW(runSemantic("basic16.in"));
}
TEST(semantic1, basic17) {
    EXPECT_NO_THROW(runSemantic("basic17.in"));
}
TEST(IR, basic17) {
    EXPECT_NO_THROW(runIr("basic17.in"));
}
TEST(semantic1, basic18) {
    EXPECT_NO_THROW(runSemantic("basic18.in"));
}
TEST(IR, basic18) {
    EXPECT_NO_THROW(runIr("basic18.in"));
}
TEST(semantic1, basic19) {
    EXPECT_NO_THROW(runSemantic("basic19.in"));
}
TEST(IR, basic19) {
    EXPECT_NO_THROW(runIr("basic19.in"));
}
TEST(semantic1, basic2) {
    EXPECT_ANY_THROW(runSemantic("basic2.in"));
}
TEST(semantic1, basic20) {
    EXPECT_NO_THROW(runSemantic("basic20.in"));
}
TEST(IR, basic20) {
    EXPECT_NO_THROW(runIr("basic20.in"));
}
TEST(semantic1, basic21) {
    EXPECT_NO_THROW(runSemantic("basic21.in"));
}
TEST(IR, basic21) {
    EXPECT_NO_THROW(runIr("basic21.in"));
}
TEST(semantic1, basic22) {
    EXPECT_NO_THROW(runSemantic("basic22.in"));
}
TEST(IR, basic22) {
    EXPECT_NO_THROW(runIr("basic22.in"));
}
TEST(semantic1, basic23) {
    EXPECT_NO_THROW(runSemantic("basic23.in"));
}
TEST(IR, basic23) {
    EXPECT_NO_THROW(runIr("basic23.in"));
}
TEST(semantic1, basic24) {
    EXPECT_NO_THROW(runSemantic("basic24.in"));
}
TEST(IR, basic24) {
    EXPECT_NO_THROW(runIr("basic24.in"));
}
TEST(semantic1, basic25) {
    EXPECT_NO_THROW(runSemantic("basic25.in"));
}
TEST(IR, basic25) {
    EXPECT_NO_THROW(runIr("basic25.in"));
}
TEST(semantic1, basic26) {
    EXPECT_NO_THROW(runSemantic("basic26.in"));
}
TEST(IR, basic26) {
    EXPECT_NO_THROW(runIr("basic26.in"));
}
TEST(semantic1, basic27) {
    EXPECT_NO_THROW(runSemantic("basic27.in"));
}
TEST(IR, basic27) {
    EXPECT_NO_THROW(runIr("basic27.in"));
}
TEST(semantic1, basic28) {
    EXPECT_ANY_THROW(runSemantic("basic28.in"));
}
TEST(semantic1, basic29) {
    EXPECT_NO_THROW(runSemantic("basic29.in"));
}
TEST(IR, basic29) {
    EXPECT_NO_THROW(runIr("basic29.in"));
}
TEST(semantic1, basic3) {
    EXPECT_ANY_THROW(runSemantic("basic3.in"));
}
TEST(semantic1, basic30) {
    EXPECT_ANY_THROW(runSemantic("basic30.in"));
}
TEST(semantic1, basic31) {
    EXPECT_ANY_THROW(runSemantic("basic31.in"));
}
TEST(semantic1, basic32) {
    EXPECT_ANY_THROW(runSemantic("basic32.in"));
}
TEST(semantic1, basic33) {
    EXPECT_ANY_THROW(runSemantic("basic33.in"));
}
TEST(semantic1, basic34) {
    EXPECT_ANY_THROW(runSemantic("basic34.in"));
}
TEST(semantic1, basic35) {
    EXPECT_ANY_THROW(runSemantic("basic35.in"));
}
TEST(semantic1, basic36) {
    EXPECT_NO_THROW(runSemantic("basic36.in"));
}
TEST(IR, basic36) {
    EXPECT_NO_THROW(runIr("basic36.in"));
}
TEST(semantic1, basic37) {
    EXPECT_NO_THROW(runSemantic("basic37.in"));
}
TEST(IR, basic37) {
    EXPECT_NO_THROW(runIr("basic37.in"));
}
TEST(semantic1, basic38) {
    EXPECT_ANY_THROW(runSemantic("basic38.in"));
}
TEST(semantic1, basic39) {
    EXPECT_NO_THROW(runSemantic("basic39.in"));
}
TEST(IR, basic39) {
    EXPECT_NO_THROW(runIr("basic39.in"));
}
TEST(semantic1, basic4) {
    EXPECT_NO_THROW(runSemantic("basic4.in"));
}
TEST(IR, basic4) {
    EXPECT_NO_THROW(runIr("basic4.in"));
}
TEST(semantic1, basic40) {
    EXPECT_NO_THROW(runSemantic("basic40.in"));
}
TEST(IR, basic40) {
    EXPECT_NO_THROW(runIr("basic40.in"));
}
TEST(semantic1, basic5) {
    EXPECT_ANY_THROW(runSemantic("basic5.in"));
}
TEST(semantic1, basic6) {
    EXPECT_ANY_THROW(runSemantic("basic6.in"));
}
TEST(semantic1, basic7) {
    EXPECT_ANY_THROW(runSemantic("basic7.in"));
}
TEST(semantic1, basic8) {
    EXPECT_ANY_THROW(runSemantic("basic8.in"));
}
TEST(semantic1, basic9) {
    EXPECT_ANY_THROW(runSemantic("basic9.in"));
}
TEST(semantic1, expr1) {
    EXPECT_ANY_THROW(runSemantic("expr1.in"));
}
TEST(semantic1, expr10) {
    EXPECT_ANY_THROW(runSemantic("expr10.in"));
}
TEST(semantic1, expr11) {
    EXPECT_ANY_THROW(runSemantic("expr11.in"));
}
TEST(semantic1, expr12) {
    EXPECT_ANY_THROW(runSemantic("expr12.in"));
}
TEST(semantic1, expr13) {
    EXPECT_NO_THROW(runSemantic("expr13.in"));
}
TEST(IR, expr13) {
    EXPECT_NO_THROW(runIr("expr13.in"));
}
TEST(semantic1, expr14) {
    EXPECT_ANY_THROW(runSemantic("expr14.in"));
}
TEST(semantic1, expr15) {
    EXPECT_ANY_THROW(runSemantic("expr15.in"));
}
TEST(semantic1, expr16) {
    EXPECT_ANY_THROW(runSemantic("expr16.in"));
}
TEST(semantic1, expr17) {
    EXPECT_NO_THROW(runSemantic("expr17.in"));
}
TEST(IR, expr17) {
    EXPECT_NO_THROW(runIr("expr17.in"));
}
TEST(semantic1, expr18) {
    EXPECT_ANY_THROW(runSemantic("expr18.in"));
}
TEST(semantic1, expr19) {
    EXPECT_NO_THROW(runSemantic("expr19.in"));
}
TEST(IR, expr19) {
    EXPECT_NO_THROW(runIr("expr19.in"));
}
TEST(semantic1, expr2) {
    EXPECT_ANY_THROW(runSemantic("expr2.in"));
}
TEST(semantic1, expr20) {
    EXPECT_ANY_THROW(runSemantic("expr20.in"));
}
TEST(semantic1, expr21) {
    EXPECT_ANY_THROW(runSemantic("expr21.in"));
}
TEST(semantic1, expr22) {
    EXPECT_ANY_THROW(runSemantic("expr22.in"));
}
TEST(semantic1, expr23) {
    EXPECT_ANY_THROW(runSemantic("expr23.in"));
}
TEST(semantic1, expr24) {
    EXPECT_ANY_THROW(runSemantic("expr24.in"));
}
TEST(semantic1, expr25) {
    EXPECT_ANY_THROW(runSemantic("expr25.in"));
}
TEST(semantic1, expr26) {
    EXPECT_ANY_THROW(runSemantic("expr26.in"));
}
TEST(semantic1, expr27) {
    EXPECT_ANY_THROW(runSemantic("expr27.in"));
}
TEST(semantic1, expr28) {
    EXPECT_ANY_THROW(runSemantic("expr28.in"));
}
TEST(semantic1, expr29) {
    EXPECT_ANY_THROW(runSemantic("expr29.in"));
}
TEST(semantic1, expr3) {
    EXPECT_ANY_THROW(runSemantic("expr3.in"));
}
TEST(semantic1, expr30) {
    EXPECT_ANY_THROW(runSemantic("expr30.in"));
}
TEST(semantic1, expr31) {
    EXPECT_ANY_THROW(runSemantic("expr31.in"));
}
TEST(semantic1, expr32) {
    EXPECT_ANY_THROW(runSemantic("expr32.in"));
}
TEST(semantic1, expr33) {
    EXPECT_NO_THROW(runSemantic("expr33.in"));
}
TEST(IR, expr33) {
    EXPECT_NO_THROW(runIr("expr33.in"));
}
TEST(semantic1, expr34) {
    EXPECT_NO_THROW(runSemantic("expr34.in"));
}
TEST(IR, expr34) {
    EXPECT_NO_THROW(runIr("expr34.in"));
}
TEST(semantic1, expr35) {
    EXPECT_ANY_THROW(runSemantic("expr35.in"));
}
TEST(semantic1, expr36) {
    EXPECT_NO_THROW(runSemantic("expr36.in"));
}
TEST(IR, expr36) {
    EXPECT_NO_THROW(runIr("expr36.in"));
}
TEST(semantic1, expr37) {
    EXPECT_ANY_THROW(runSemantic("expr37.in"));
}
TEST(semantic1, expr38) {
    EXPECT_NO_THROW(runSemantic("expr38.in"));
}
TEST(IR, expr38) {
    EXPECT_NO_THROW(runIr("expr38.in"));
}
TEST(semantic1, expr39) {
    EXPECT_ANY_THROW(runSemantic("expr39.in"));
}
TEST(semantic1, expr4) {
    EXPECT_ANY_THROW(runSemantic("expr4.in"));
}
TEST(semantic1, expr40) {
    EXPECT_ANY_THROW(runSemantic("expr40.in"));
}
TEST(semantic1, expr5) {
    EXPECT_ANY_THROW(runSemantic("expr5.in"));
}
TEST(semantic1, expr6) {
    EXPECT_ANY_THROW(runSemantic("expr6.in"));
}
TEST(semantic1, expr7) {
    EXPECT_ANY_THROW(runSemantic("expr7.in"));
}
TEST(semantic1, expr8) {
    EXPECT_ANY_THROW(runSemantic("expr8.in"));
}
TEST(semantic1, expr9) {
    EXPECT_ANY_THROW(runSemantic("expr9.in"));
}
TEST(semantic1, if1) {
    EXPECT_NO_THROW(runSemantic("if1.in"));
}
TEST(IR, if1) {
    EXPECT_NO_THROW(runIr("if1.in"));
}
TEST(semantic1, if10) {
    EXPECT_NO_THROW(runSemantic("if10.in"));
}
TEST(IR, if10) {
    EXPECT_NO_THROW(runIr("if10.in"));
}
TEST(semantic1, if11) {
    EXPECT_ANY_THROW(runSemantic("if11.in"));
}
TEST(semantic1, if12) {
    EXPECT_ANY_THROW(runSemantic("if12.in"));
}
TEST(semantic1, if13) {
    EXPECT_ANY_THROW(runSemantic("if13.in"));
}
TEST(semantic1, if14) {
    EXPECT_ANY_THROW(runSemantic("if14.in"));
}
TEST(semantic1, if15) {
    EXPECT_ANY_THROW(runSemantic("if15.in"));
}
TEST(semantic1, if2) {
    EXPECT_NO_THROW(runSemantic("if2.in"));
}
TEST(IR, if2) {
    EXPECT_NO_THROW(runIr("if2.in"));
}
TEST(semantic1, if3) {
    EXPECT_NO_THROW(runSemantic("if3.in"));
}
TEST(IR, if3) {
    EXPECT_NO_THROW(runIr("if3.in"));
}
TEST(semantic1, if4) {
    EXPECT_NO_THROW(runSemantic("if4.in"));
}
TEST(IR, if4) {
    EXPECT_NO_THROW(runIr("if4.in"));
}
TEST(semantic1, if5) {
    EXPECT_NO_THROW(runSemantic("if5.in"));
}
TEST(IR, if5) {
    EXPECT_NO_THROW(runIr("if5.in"));
}
TEST(semantic1, if6) {
    EXPECT_NO_THROW(runSemantic("if6.in"));
}
TEST(IR, if6) {
    EXPECT_NO_THROW(runIr("if6.in"));
}
TEST(semantic1, if7) {
    EXPECT_NO_THROW(runSemantic("if7.in"));
}
TEST(IR, if7) {
    EXPECT_NO_THROW(runIr("if7.in"));
}
TEST(semantic1, if8) {
    EXPECT_NO_THROW(runSemantic("if8.in"));
}
TEST(IR, if8) {
    EXPECT_NO_THROW(runIr("if8.in"));
}
TEST(semantic1, if9) {
    EXPECT_NO_THROW(runSemantic("if9.in"));
}
TEST(IR, if9) {
    EXPECT_NO_THROW(runIr("if9.in"));
}
TEST(semantic1, loop1) {
    EXPECT_NO_THROW(runSemantic("loop1.in"));
}
TEST(IR, loop1) {
    EXPECT_NO_THROW(runIr("loop1.in"));
}
TEST(semantic1, loop10) {
    EXPECT_ANY_THROW(runSemantic("loop10.in"));
}
TEST(semantic1, loop2) {
    EXPECT_NO_THROW(runSemantic("loop2.in"));
}
TEST(IR, loop2) {
    EXPECT_NO_THROW(runIr("loop2.in"));
}
TEST(semantic1, loop3) {
    EXPECT_NO_THROW(runSemantic("loop3.in"));
}
TEST(IR, loop3) {
    EXPECT_NO_THROW(runIr("loop3.in"));
}
TEST(semantic1, loop4) {
    EXPECT_NO_THROW(runSemantic("loop4.in"));
}
TEST(IR, loop4) {
    EXPECT_NO_THROW(runIr("loop4.in"));
}
TEST(semantic1, loop5) {
    EXPECT_NO_THROW(runSemantic("loop5.in"));
}
TEST(IR, loop5) {
    EXPECT_NO_THROW(runIr("loop5.in"));
}
TEST(semantic1, loop6) {
    EXPECT_ANY_THROW(runSemantic("loop6.in"));
}
TEST(semantic1, loop7) {
    EXPECT_ANY_THROW(runSemantic("loop7.in"));
}
TEST(semantic1, loop8) {
    EXPECT_ANY_THROW(runSemantic("loop8.in"));
}
TEST(semantic1, loop9) {
    EXPECT_ANY_THROW(runSemantic("loop9.in"));
}
TEST(semantic1, misc1) {
    EXPECT_NO_THROW(runSemantic("misc1.in"));
}
TEST(IR, misc1) {
    EXPECT_NO_THROW(runIr("misc1.in"));
}
TEST(semantic1, misc10) {
    EXPECT_NO_THROW(runSemantic("misc10.in"));
}
TEST(IR, misc10) {
    EXPECT_NO_THROW(runIr("misc10.in"));
}
TEST(semantic1, misc11) {
    EXPECT_NO_THROW(runSemantic("misc11.in"));
}
TEST(IR, misc11) {
    EXPECT_NO_THROW(runIr("misc11.in"));
}
TEST(semantic1, misc12) {
    EXPECT_NO_THROW(runSemantic("misc12.in"));
}
TEST(IR, misc12) {
    EXPECT_NO_THROW(runIr("misc12.in"));
}
TEST(semantic1, misc13) {
    EXPECT_NO_THROW(runSemantic("misc13.in"));
}
TEST(IR, misc13) {
    EXPECT_NO_THROW(runIr("misc13.in"));
}
TEST(semantic1, misc14) {
    EXPECT_NO_THROW(runSemantic("misc14.in"));
}
TEST(IR, misc14) {
    EXPECT_NO_THROW(runIr("misc14.in"));
}
TEST(semantic1, misc15) {
    EXPECT_NO_THROW(runSemantic("misc15.in"));
}
TEST(IR, misc15) {
    EXPECT_NO_THROW(runIr("misc15.in"));
}
TEST(semantic1, misc16) {
    EXPECT_NO_THROW(runSemantic("misc16.in"));
}
TEST(IR, misc16) {
    EXPECT_NO_THROW(runIr("misc16.in"));
}
TEST(semantic1, misc17) {
    EXPECT_NO_THROW(runSemantic("misc17.in"));
}
TEST(IR, misc17) {
    EXPECT_NO_THROW(runIr("misc17.in"));
}
TEST(semantic1, misc18) {
    EXPECT_NO_THROW(runSemantic("misc18.in"));
}
TEST(IR, misc18) {
    EXPECT_NO_THROW(runIr("misc18.in"));
}
TEST(semantic1, misc19) {
    EXPECT_NO_THROW(runSemantic("misc19.in"));
}
TEST(IR, misc19) {
    EXPECT_NO_THROW(runIr("misc19.in"));
}
TEST(semantic1, misc2) {
    EXPECT_NO_THROW(runSemantic("misc2.in"));
}
TEST(IR, misc2) {
    EXPECT_NO_THROW(runIr("misc2.in"));
}
TEST(semantic1, misc20) {
    EXPECT_NO_THROW(runSemantic("misc20.in"));
}
TEST(IR, misc20) {
    EXPECT_NO_THROW(runIr("misc20.in"));
}
TEST(semantic1, misc21) {
    EXPECT_NO_THROW(runSemantic("misc21.in"));
}
TEST(IR, misc21) {
    EXPECT_NO_THROW(runIr("misc21.in"));
}
TEST(semantic1, misc22) {
    EXPECT_NO_THROW(runSemantic("misc22.in"));
}
TEST(IR, misc22) {
    EXPECT_NO_THROW(runIr("misc22.in"));
}
TEST(semantic1, misc23) {
    EXPECT_NO_THROW(runSemantic("misc23.in"));
}
TEST(IR, misc23) {
    EXPECT_NO_THROW(runIr("misc23.in"));
}
TEST(semantic1, misc24) {
    EXPECT_NO_THROW(runSemantic("misc24.in"));
}
TEST(IR, misc24) {
    EXPECT_NO_THROW(runIr("misc24.in"));
}
TEST(semantic1, misc25) {
    EXPECT_NO_THROW(runSemantic("misc25.in"));
}
TEST(IR, misc25) {
    EXPECT_NO_THROW(runIr("misc25.in"));
}
TEST(semantic1, misc26) {
    EXPECT_NO_THROW(runSemantic("misc26.in"));
}
TEST(IR, misc26) {
    EXPECT_NO_THROW(runIr("misc26.in"));
}
TEST(semantic1, misc27) {
    EXPECT_NO_THROW(runSemantic("misc27.in"));
}
TEST(IR, misc27) {
    EXPECT_NO_THROW(runIr("misc27.in"));
}
TEST(semantic1, misc28) {
    EXPECT_NO_THROW(runSemantic("misc28.in"));
}
TEST(IR, misc28) {
    EXPECT_NO_THROW(runIr("misc28.in"));
}
TEST(semantic1, misc29) {
    EXPECT_NO_THROW(runSemantic("misc29.in"));
}
TEST(IR, misc29) {
    EXPECT_NO_THROW(runIr("misc29.in"));
}
TEST(semantic1, misc3) {
    EXPECT_NO_THROW(runSemantic("misc3.in"));
}
TEST(IR, misc3) {
    EXPECT_NO_THROW(runIr("misc3.in"));
}
TEST(semantic1, misc30) {
    EXPECT_NO_THROW(runSemantic("misc30.in"));
}
TEST(IR, misc30) {
    EXPECT_NO_THROW(runIr("misc30.in"));
}
TEST(semantic1, misc31) {
    EXPECT_NO_THROW(runSemantic("misc31.in"));
}
TEST(IR, misc31) {
    EXPECT_NO_THROW(runIr("misc31.in"));
}
TEST(semantic1, misc32) {
    EXPECT_NO_THROW(runSemantic("misc32.in"));
}
TEST(IR, misc32) {
    EXPECT_NO_THROW(runIr("misc32.in"));
}
TEST(semantic1, misc33) {
    EXPECT_NO_THROW(runSemantic("misc33.in"));
}
TEST(IR, misc33) {
    EXPECT_NO_THROW(runIr("misc33.in"));
}
TEST(semantic1, misc34) {
    EXPECT_NO_THROW(runSemantic("misc34.in"));
}
TEST(IR, misc34) {
    EXPECT_NO_THROW(runIr("misc34.in"));
}
TEST(semantic1, misc35) {
    EXPECT_NO_THROW(runSemantic("misc35.in"));
}
TEST(IR, misc35) {
    EXPECT_NO_THROW(runIr("misc35.in"));
}
TEST(semantic1, misc36) {
    EXPECT_NO_THROW(runSemantic("misc36.in"));
}
TEST(IR, misc36) {
    EXPECT_NO_THROW(runIr("misc36.in"));
}
TEST(semantic1, misc37) {
    EXPECT_NO_THROW(runSemantic("misc37.in"));
}
TEST(IR, misc37) {
    EXPECT_NO_THROW(runIr("misc37.in"));
}
TEST(semantic1, misc38) {
    EXPECT_NO_THROW(runSemantic("misc38.in"));
}
TEST(IR, misc38) {
    EXPECT_NO_THROW(runIr("misc38.in"));
}
TEST(semantic1, misc39) {
    EXPECT_NO_THROW(runSemantic("misc39.in"));
}
TEST(IR, misc39) {
    EXPECT_NO_THROW(runIr("misc39.in"));
}
TEST(semantic1, misc4) {
    EXPECT_NO_THROW(runSemantic("misc4.in"));
}
TEST(IR, misc4) {
    EXPECT_NO_THROW(runIr("misc4.in"));
}
TEST(semantic1, misc40) {
    EXPECT_NO_THROW(runSemantic("misc40.in"));
}
TEST(IR, misc40) {
    EXPECT_NO_THROW(runIr("misc40.in"));
}
TEST(semantic1, misc41) {
    EXPECT_NO_THROW(runSemantic("misc41.in"));
}
TEST(IR, misc41) {
    EXPECT_NO_THROW(runIr("misc41.in"));
}
TEST(semantic1, misc42) {
    EXPECT_NO_THROW(runSemantic("misc42.in"));
}
TEST(IR, misc42) {
    EXPECT_NO_THROW(runIr("misc42.in"));
}
TEST(semantic1, misc43) {
    EXPECT_NO_THROW(runSemantic("misc43.in"));
}
TEST(IR, misc43) {
    EXPECT_NO_THROW(runIr("misc43.in"));
}
TEST(semantic1, misc44) {
    EXPECT_NO_THROW(runSemantic("misc44.in"));
}
TEST(IR, misc44) {
    EXPECT_NO_THROW(runIr("misc44.in"));
}
TEST(semantic1, misc45) {
    EXPECT_NO_THROW(runSemantic("misc45.in"));
}
TEST(IR, misc45) {
    EXPECT_NO_THROW(runIr("misc45.in"));
}
TEST(semantic1, misc46) {
    EXPECT_NO_THROW(runSemantic("misc46.in"));
}
TEST(IR, misc46) {
    EXPECT_NO_THROW(runIr("misc46.in"));
}
TEST(semantic1, misc47) {
    EXPECT_NO_THROW(runSemantic("misc47.in"));
}
TEST(IR, misc47) {
    EXPECT_NO_THROW(runIr("misc47.in"));
}
TEST(semantic1, misc48) {
    EXPECT_NO_THROW(runSemantic("misc48.in"));
}
TEST(IR, misc48) {
    EXPECT_NO_THROW(runIr("misc48.in"));
}
TEST(semantic1, misc49) {
    EXPECT_NO_THROW(runSemantic("misc49.in"));
}
TEST(IR, misc49) {
    EXPECT_NO_THROW(runIr("misc49.in"));
}
TEST(semantic1, misc5) {
    EXPECT_NO_THROW(runSemantic("misc5.in"));
}
TEST(IR, misc5) {
    EXPECT_NO_THROW(runIr("misc5.in"));
}
TEST(semantic1, misc50) {
    EXPECT_NO_THROW(runSemantic("misc50.in"));
}
TEST(IR, misc50) {
    EXPECT_NO_THROW(runIr("misc50.in"));
}
TEST(semantic1, misc51) {
    EXPECT_NO_THROW(runSemantic("misc51.in"));
}
TEST(IR, misc51) {
    EXPECT_NO_THROW(runIr("misc51.in"));
}
TEST(semantic1, misc52) {
    EXPECT_NO_THROW(runSemantic("misc52.in"));
}
TEST(IR, misc52) {
    EXPECT_NO_THROW(runIr("misc52.in"));
}
TEST(semantic1, misc53) {
    EXPECT_NO_THROW(runSemantic("misc53.in"));
}
TEST(IR, misc53) {
    EXPECT_NO_THROW(runIr("misc53.in"));
}
TEST(semantic1, misc54) {
    EXPECT_NO_THROW(runSemantic("misc54.in"));
}
TEST(IR, misc54) {
    EXPECT_NO_THROW(runIr("misc54.in"));
}
TEST(semantic1, misc55) {
    EXPECT_NO_THROW(runSemantic("misc55.in"));
}
TEST(IR, misc55) {
    EXPECT_NO_THROW(runIr("misc55.in"));
}
TEST(semantic1, misc56) {
    EXPECT_ANY_THROW(runSemantic("misc56.in"));
}
TEST(semantic1, misc57) {
    EXPECT_ANY_THROW(runSemantic("misc57.in"));
}
TEST(semantic1, misc58) {
    EXPECT_ANY_THROW(runSemantic("misc58.in"));
}
TEST(semantic1, misc59) {
    EXPECT_ANY_THROW(runSemantic("misc59.in"));
}
TEST(semantic1, misc6) {
    EXPECT_NO_THROW(runSemantic("misc6.in"));
}
TEST(IR, misc6) {
    EXPECT_NO_THROW(runIr("misc6.in"));
}
TEST(semantic1, misc60) {
    EXPECT_ANY_THROW(runSemantic("misc60.in"));
}
TEST(semantic1, misc61) {
    EXPECT_ANY_THROW(runSemantic("misc61.in"));
}
TEST(semantic1, misc62) {
    EXPECT_ANY_THROW(runSemantic("misc62.in"));
}
TEST(semantic1, misc63) {
    EXPECT_ANY_THROW(runSemantic("misc63.in"));
}
TEST(semantic1, misc64) {
    EXPECT_ANY_THROW(runSemantic("misc64.in"));
}
TEST(semantic1, misc65) {
    EXPECT_NO_THROW(runSemantic("misc65.in"));
}
TEST(IR, misc65) {
    EXPECT_NO_THROW(runIr("misc65.in"));
}
TEST(semantic1, misc7) {
    EXPECT_NO_THROW(runSemantic("misc7.in"));
}
TEST(IR, misc7) {
    EXPECT_NO_THROW(runIr("misc7.in"));
}
TEST(semantic1, misc8) {
    EXPECT_NO_THROW(runSemantic("misc8.in"));
}
TEST(IR, misc8) {
    EXPECT_NO_THROW(runIr("misc8.in"));
}
TEST(semantic1, misc9) {
    EXPECT_NO_THROW(runSemantic("misc9.in"));
}
TEST(IR, misc9) {
    EXPECT_NO_THROW(runIr("misc9.in"));
}
TEST(semantic1, return1) {
    EXPECT_ANY_THROW(runSemantic("return1.in"));
}
TEST(semantic1, return10) {
    EXPECT_ANY_THROW(runSemantic("return10.in"));
}
TEST(semantic1, return11) {
    EXPECT_ANY_THROW(runSemantic("return11.in"));
}
TEST(semantic1, return12) {
    EXPECT_NO_THROW(runSemantic("return12.in"));
}
TEST(IR, return12) {
    EXPECT_NO_THROW(runIr("return12.in"));
}
TEST(semantic1, return13) {
    EXPECT_NO_THROW(runSemantic("return13.in"));
}
TEST(IR, return13) {
    EXPECT_NO_THROW(runIr("return13.in"));
}
TEST(semantic1, return14) {
    EXPECT_NO_THROW(runSemantic("return14.in"));
}
TEST(IR, return14) {
    EXPECT_NO_THROW(runIr("return14.in"));
}
TEST(semantic1, return15) {
    EXPECT_NO_THROW(runSemantic("return15.in"));
}
TEST(IR, return15) {
    EXPECT_NO_THROW(runIr("return15.in"));
}
TEST(semantic1, return2) {
    EXPECT_NO_THROW(runSemantic("return2.in"));
}
TEST(IR, return2) {
    EXPECT_NO_THROW(runIr("return2.in"));
}
TEST(semantic1, return3) {
    EXPECT_ANY_THROW(runSemantic("return3.in"));
}
TEST(semantic1, return4) {
    EXPECT_ANY_THROW(runSemantic("return4.in"));
}
TEST(semantic1, return5) {
    EXPECT_ANY_THROW(runSemantic("return5.in"));
}
TEST(semantic1, return6) {
    EXPECT_ANY_THROW(runSemantic("return6.in"));
}
TEST(semantic1, return7) {
    EXPECT_ANY_THROW(runSemantic("return7.in"));
}
TEST(semantic1, return8) {
    EXPECT_NO_THROW(runSemantic("return8.in"));
}
TEST(IR, return8) {
    EXPECT_NO_THROW(runIr("return8.in"));
}
TEST(semantic1, return9) {
    EXPECT_ANY_THROW(runSemantic("return9.in"));
}
TEST(semantic1, type1) {
    EXPECT_ANY_THROW(runSemantic("type1.in"));
}
TEST(semantic1, type10) {
    EXPECT_ANY_THROW(runSemantic("type10.in"));
}
TEST(semantic1, type11) {
    EXPECT_ANY_THROW(runSemantic("type11.in"));
}
TEST(semantic1, type12) {
    EXPECT_ANY_THROW(runSemantic("type12.in"));
}
TEST(semantic1, type13) {
    EXPECT_ANY_THROW(runSemantic("type13.in"));
}
TEST(semantic1, type14) {
    EXPECT_ANY_THROW(runSemantic("type14.in"));
}
TEST(semantic1, type15) {
    EXPECT_ANY_THROW(runSemantic("type15.in"));
}
TEST(semantic1, type16) {
    EXPECT_ANY_THROW(runSemantic("type16.in"));
}
TEST(semantic1, type17) {
    EXPECT_ANY_THROW(runSemantic("type17.in"));
}
TEST(semantic1, type18) {
    EXPECT_ANY_THROW(runSemantic("type18.in"));
}
TEST(semantic1, type19) {
    EXPECT_ANY_THROW(runSemantic("type19.in"));
}
TEST(semantic1, type2) {
    EXPECT_ANY_THROW(runSemantic("type2.in"));
}
TEST(semantic1, type20) {
    EXPECT_ANY_THROW(runSemantic("type20.in"));
}
TEST(semantic1, type3) {
    EXPECT_ANY_THROW(runSemantic("type3.in"));
}
TEST(semantic1, type4) {
    EXPECT_ANY_THROW(runSemantic("type4.in"));
}
TEST(semantic1, type5) {
    EXPECT_ANY_THROW(runSemantic("type5.in"));
}
TEST(semantic1, type6) {
    EXPECT_ANY_THROW(runSemantic("type6.in"));
}
TEST(semantic1, type7) {
    EXPECT_ANY_THROW(runSemantic("type7.in"));
}
TEST(semantic1, type8) {
    EXPECT_ANY_THROW(runSemantic("type8.in"));
}
TEST(semantic1, type9) {
    EXPECT_ANY_THROW(runSemantic("type9.in"));
}
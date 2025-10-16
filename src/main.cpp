#include<bits/stdc++.h>
#include"parser.h"
#include"ir.h"
using namespace std;
int main()
{
    freopen("my.in","r",stdin);
    freopen("my.ll","w",stdout);
    int in;
    string code;
    while((in=cin.get())!=EOF)
        code.push_back(static_cast<char>(in));
    code.push_back('\n');
    auto par=parser(code);
    astNode* root;
    try
    {
        root=par.solve();
        semanticCheck(root);
        auto irCode=generateIr(root);
        auto res=root->showTree();
        for (const auto& t:res)
            std::cerr<<t<<std::endl;
        std::cout<<std::endl;
        for (const auto& t:irCode)
            std::cout<<t<<std::endl;
    }
    catch (std::bad_any_cast&)
    {
        std::cout<<"Bad any cast!"<<std::endl;
    }
    catch (std::bad_alloc&)
    {
        std::cout<<"Bad alloc!"<<std::endl;
    }
    catch (compileError&)
    {
        std::cout<<"Compile Error!"<<std::endl;
    }
    auto cl=system("clang -S --target=riscv32-unknown-elf -march=rv32gc -mabi=ilp32d -O0 my.ll -o my.s");
    std::cerr<<"clang check result: "<<cl<<std::endl;

    return 0;
}
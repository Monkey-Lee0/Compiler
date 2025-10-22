#include<bits/stdc++.h>
#include"parser.h"
#include"ir.h"
using namespace std;
constexpr int nodeMode = false;
int main()
{
    ifstream ifs("my.in");
    ofstream ofs("my.ll");
    int in;
    string code;
    while((in=ifs.get())!=EOF)
        code.push_back(static_cast<char>(in));
    code.push_back('\n');
    auto par=parser(code);
    astNode* root;
    try
    {
        root=par.solve();
        semanticCheck(root);
        auto irCode=generateIr(root);
        if (nodeMode)
        {
            auto res=root->showTree();
            for (const auto& t:res)
                std::cerr<<t<<std::endl;
            std::cerr<<std::endl;
        }
        ofs<<std::endl;
        for (const auto& t:irCode)
            ofs<<t<<std::endl;
    }
    catch (std::bad_any_cast&)
    {
        std::cout<<"Bad any cast!"<<std::endl;
        return 0;
    }
    catch (std::bad_alloc&)
    {
        std::cout<<"Bad alloc!"<<std::endl;
        return 0;
    }
    catch (compileError&)
    {
        std::cout<<"Compile Error!"<<std::endl;
        return 0;
    }
    auto cl=system("clang --target=x86_64-unknown-linux-gnu -march=x86-64 -O0 my.ll -o my");
    std::cout<<"clang check result: "<<cl<<std::endl;
    if (cl)
        return 0;
    std::cout<<"running result:"<<std::endl;
    system("./my");

    return 0;
}
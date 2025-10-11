#include<bits/stdc++.h>
#include"parser.h"
#include"ir.h"
using namespace std;
int main()
{
    freopen("my.in","r",stdin);
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
        std::cout<<std::endl;
    }
    catch(...)
    {
        std::cout<<"Compile Error!"<<std::endl;
    }

    return 0;
}
#include<bits/stdc++.h>
#include"parser.h"
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
    try
    {
        auto res=par.solve()->showTree();
        for (const auto& t:res)
                std::cout<<t<<std::endl;
    }
    catch(...)
    {
        std::cout<<"Compile Error!"<<std::endl;
    }

    return 0;
}
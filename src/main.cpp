#include<bits/stdc++.h>
#include"lexer.h"
using namespace std;
int main()
{
    freopen("my.in","r",stdin);
    int in;
    string code;
    while((in=cin.get())!=EOF)
        code.push_back(static_cast<char>(in));
    code.push_back('\n');
    lexer lexer_(code);
    try
    {
        while(true)
        {
            auto t=lexer_.consume();
            if(t.type==tokenType::ILLEGAL)
                break;
            std::cout<<t<<std::endl;
        }
    }
    catch(...)
    {
        std::cout<<"Compile Error!"<<std::endl;
    }

    return 0;
}
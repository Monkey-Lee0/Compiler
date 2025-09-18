#ifndef TOKEN_H
#define TOKEN_H

#include<string>
#include<iostream>

enum class tokenType
{
    ILLEGAL,CHAR_LITERAL,RAW_STRING_LITERAL,STRING_LITERAL,INTEGER_LITERAL,FLOAT_LITERAL,KEYWORD,OPERATOR,IDENTIFIER
};

inline std::string tokenTypeToString(tokenType);

class token
{
public:
    tokenType type;
    std::string value;
    friend bool operator==(const token &a,const token &b){return a.type==b.type && a.value==b.value;}
    friend bool operator!=(const token &a,const token &b){return !(a==b);}
};

std::ostream& operator<<(std::ostream&,const token&);

class compileError final:public std::exception
{
    std::string msg;
public:
    compileError()=default;
    explicit compileError(const std::string&);
    const char* what() const noexcept override;
};


#endif
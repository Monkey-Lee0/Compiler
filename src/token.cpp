#include "token.h"

inline std::string tokenTypeToString(const tokenType type)
{
    if(type==tokenType::CHAR_LITERAL)
        return "CHAR_LITERAL";
    if(type==tokenType::RAW_STRING_LITERAL)
        return "RAW_STRING_LITERAL";
    if(type==tokenType::STRING_LITERAL)
        return "STRING_LITERAL";
    if(type==tokenType::INTEGER_LITERAL)
        return "INTEGER_LITERAL";
    if(type==tokenType::FLOAT_LITERAL)
        return "FLOAT_LITERAL";
    if(type==tokenType::KEYWORD)
        return "KEYWORD";
    if(type==tokenType::OPERATOR)
        return "OPERATOR";
    if(type==tokenType::IDENTIFIER)
        return "IDENTIFIER";
    return "ILLEGAL";
}

std::ostream& operator<<(std::ostream& os,const token& t)
{
    return os<<"<"<<tokenTypeToString(t.type)<<",\""<<t.value<<"\">";
}

const char* compileError::what() const noexcept{return msg.data();}

compileError::compileError(const std::string& str):msg(str){}

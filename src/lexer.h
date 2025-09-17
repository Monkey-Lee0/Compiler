#ifndef LEXER_H
#define LEXER_H

#include "token.h"

class lexer
{
    int matchWhitespace(int) const;
    int matchSingleChar(int) const;
    int matchCharLiteral(int) const;
    int matchRawStringLiteral(int) const;
    int matchStringLiteral(int) const;
    int matchIntegerLiteral(int) const;
    int matchFloatLiteral(int) const;
    int matchKeyword(int) const;
    int matchOperator(int) const;
    int matchIdentifier(int) const;
public:
    std::string code;
    int pos;
    explicit lexer(const std::string&);
    token consume();
    token peek() const;
    token expect(const tokenType&) const;
    bool checkEnd() const;
};

#endif
#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "lexer.h"
#include "ast.h"

class parser
{
    lexer src;
    void appendProgram(astNode*);
    void appendFunction(astNode*);
    void appendStatementBlock(astNode*);
    void appendLetStatement(astNode*);
    void appendExpressionStatement(astNode*);
    void appendParameters(astNode*);
    void appendTypedIdentifier(astNode*);
    void appendType(astNode*);
public:
    parser();
    explicit parser(const std::string &code);
    astNode* solve();
};

#endif
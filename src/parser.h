#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "lexer.h"
#include "ast.h"

class parser
{
    lexer src;
    void appendProgram(astNode*);
    void appendStruct(astNode*);
    void appendEnum(astNode*);
    void appendFunction(astNode*);
    void appendStatementBlock(astNode*);
    void appendLetStatement(astNode*);
    void appendConstStatement(astNode*);
    void appendExpressionStatement(astNode*);
    void appendSimpleExpression(astNode*);
    astNode* parseExp(int precedence);
    void appendGroupExpression(astNode*);
    void appendParameters(astNode*);
    void appendTypedIdentifier(astNode*);
    void appendType(astNode*);
    void appendItem(astNode*);
    void appendAssociatedItem(astNode*);
    void appendImpl(astNode*);
    void appendTrait(astNode*);
public:
    parser();
    explicit parser(const std::string &code);
    astNode* solve();
};

#endif
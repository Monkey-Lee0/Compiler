#ifndef AST_H
#define AST_H

#include<vector>
#include<string>

enum class astNodeType
{
    PROGRAM,FUNCTION,PARAMETERS,PARAMETER,STATEMENT_BLOCK,LET_STATEMENT,EXPRESSION_STATEMENT,TYPE,IDENTIFIER,
    TYPED_IDENTIFIER,BREAK,CONTINUE,RETURN,UNARY_OPERATOR,BINARY_OPERATOR,FUNCTION_CALL,CHAR_LITERAL,STRING_LITERAL,
    INTEGER_LITERAL,FLOAT_LITERAL,RAW_STRING_LITERAL,BOOL_LITERAL
};

class astNode final
{
public:
    virtual ~astNode()=default;
    std::vector<astNode*> children;
    std::string value;
    astNodeType type;
    std::vector<std::string> showSelf();
    std::vector<std::string> showTree();
};


#endif

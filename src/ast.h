#ifndef AST_H
#define AST_H

#include<vector>
#include<string>

enum class astNodeType
{
    PROGRAM,FUNCTION,PARAMETERS,PARAMETER,STATEMENT_BLOCK,LET_STATEMENT,EXPRESSION_STATEMENT,GROUP_EXPRESSION,TYPE,
    IDENTIFIER,TYPED_IDENTIFIER,BREAK,CONTINUE,RETURN,RETURN_CUR,LOOP,WHILE,IF,ELSE,UNARY_OPERATOR,BINARY_OPERATOR,
    FUNCTION_CALL,ARRAY_INDEX,ARRAY_BUILD,CHAR_LITERAL,STRING_LITERAL,INTEGER_LITERAL,FLOAT_LITERAL,RAW_STRING_LITERAL,
    BOOL_LITERAL
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

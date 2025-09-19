#ifndef AST_H
#define AST_H

#include<vector>
#include<string>

enum class astNodeType
{
    PROGRAM, STRUCT, FIELDS, FIELD, FUNCTION, PARAMETERS, QUANTIFIER, ENUM, ENUM_MEMBERS, IMPL, TRAIT,
    ASSOCIATED_ITEM, STATEMENT_BLOCK, LET_STATEMENT, CONST_STATEMENT, EXPRESSION_STATEMENT, GROUP_EXPRESSION,
    TYPE, IDENTIFIER, TYPED_IDENTIFIER, BREAK, CONTINUE, RETURN, RETURN_CUR, LOOP, WHILE, IF, ELSE, UNARY_OPERATOR,
    BINARY_OPERATOR, FUNCTION_CALL, ARRAY_INDEX, STRUCT_BUILD, ARRAY_BUILD, CHAR_LITERAL, STRING_LITERAL,
    INTEGER_LITERAL, FLOAT_LITERAL, RAW_STRING_LITERAL, BOOL_LITERAL
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

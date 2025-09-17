#ifndef AST_H
#define AST_H

#include<vector>
#include<string>

enum class astNodeType
{
    PROGRAM,FUNCTION,PARAMETERS,PARAMETER,STATEMENT_BLOCK,LET_STATEMENT,EXPRESSION_STATEMENT,TYPE,IDENTIFIER,
    TYPED_IDENTIFIER
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

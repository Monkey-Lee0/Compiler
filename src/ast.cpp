#include "ast.h"
#include<iostream>

std::vector<std::string> astNode::showSelf()
{
    std::vector<std::string> res;
    res.emplace_back("");
    switch (type)
    {
        case astNodeType::PROGRAM:
            res.back() = "program";
            break;
        case astNodeType::FUNCTION:
            res.back() = "function";
            break;
        case astNodeType::PARAMETERS:
            res.back() = "parameters";
            break;
        case astNodeType::STATEMENT_BLOCK:
            res.back() = "statement block";
            break;
        case astNodeType::LET_STATEMENT:
            res.back() = "let statement";
            break;
        case astNodeType::EXPRESSION_STATEMENT:
            res.back() = "expression statement";
            break;
        case astNodeType::TYPE:
            res.back() = "type";
            break;
        case astNodeType::IDENTIFIER:
            res.back() = "identifier";
            break;
        case astNodeType::TYPED_IDENTIFIER:
            res.back() = "typed identifier";
            break;
        default:
            ;
    }

    if (!value.empty())
        res.back().append(" : "+value);
    return res;
}

std::vector<std::string> astNode::showTree()
{
    auto res=astNode::showSelf();
    for (int i=0;i<children.size();i++)
    {
        auto resSon=children[i]->showTree();
        res.emplace_back("|");
        for (int j=0;j<resSon.size();j++)
        {
            std::string prefix="  ";
            if (j==0)
            {
                if (i+1==children.size())
                    prefix="└-";
                else
                    prefix="├-";
            }
            if (j!=0 && i+1!=children.size())
                prefix="| ";
            res.emplace_back(prefix+resSon[j]);
        }
    }

    return res;
}
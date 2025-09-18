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
        case astNodeType::GROUP_EXPRESSION:
            res.back() = "group expression";
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
        case astNodeType::BREAK:
            res.back() = "break";
            break;
        case astNodeType::CONTINUE:
            res.back() = "continue";
            break;
        case astNodeType::RETURN:
            res.back() = "return";
            break;
        case astNodeType::RETURN_CUR:
            res.back() = "return current";
            break;
        case astNodeType::LOOP:
            res.back() = "loop";
            break;
        case astNodeType::WHILE:
            res.back() = "while";
            break;
        case astNodeType::IF:
            res.back() = "if";
            break;
        case astNodeType::ELSE:
            res.back() = "else";
            break;
        case astNodeType::UNARY_OPERATOR:
            res.back() = "unary operator";
            break;
        case astNodeType::BINARY_OPERATOR:
            res.back() = "binary operator";
            break;
        case astNodeType::FUNCTION_CALL:
            res.back() = "function call";
            break;
        case astNodeType::ARRAY_INDEX:
            res.back() = "array index";
            break;
        case astNodeType::ARRAY_BUILD:
            res.back() = "array build";
            break;
        case astNodeType::CHAR_LITERAL:
            res.back() = "char literal";
            break;
        case astNodeType::STRING_LITERAL:
            res.back() = "string literal";
            break;
        case astNodeType::INTEGER_LITERAL:
            res.back() = "integer literal";
            break;
        case astNodeType::FLOAT_LITERAL:
            res.back() = "float literal";
            break;
        case astNodeType::RAW_STRING_LITERAL:
            res.back() = "raw string literal";
            break;
        case astNodeType::BOOL_LITERAL:
            res.back() = "bool literal";
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
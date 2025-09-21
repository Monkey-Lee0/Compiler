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
        case astNodeType::STRUCT:
            res.back() = "struct";
            break;
        case astNodeType::FIELDS:
            res.back() = "fields";
            break;
        case astNodeType::FIELD:
            res.back() = "field";
            break;
        case astNodeType::FUNCTION:
            res.back() = "function";
            break;
        case astNodeType::PARAMETERS:
            res.back() = "parameters";
            break;
        case astNodeType::QUANTIFIER:
            res.back() = "quantifier";
            break;
        case astNodeType::STATEMENT_BLOCK:
            res.back() = "statement block";
            break;
        case astNodeType::LET_STATEMENT:
            res.back() = "let statement";
            break;
        case astNodeType::CONST_STATEMENT:
            res.back() = "const statement";
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
        case astNodeType::ENUM:
            res.back() = "enum";
            break;
        case astNodeType::ENUM_MEMBERS:
            res.back() = "enum members";
            break;
        case astNodeType::IMPL:
            res.back() = "impl";
            break;
        case astNodeType::TRAIT:
            res.back() = "trait";
            break;
        case astNodeType::ASSOCIATED_ITEM:
            res.back() = "associated item";
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
        case astNodeType::STRUCT_BUILD:
            res.back() = "struct build";
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
        case astNodeType::SELF:
            res.back() = "self";
            break;
        default:
            ;
    }

    if (!value.empty())
        res.back().append(" : "+value);
    if (realType.name != TypeName::ILLEGAL)
        res.back().append("   type:"+realType.to_string());
    if (eval.has_value())
    {
        res.back().append("   eval:");
        if (eval.type() == typeid(long long))
        {
            auto R=std::any_cast<long long>(eval);
            if (realType == Type(TypeName::U32) || realType == Type(TypeName::USIZE) ||
                realType == Type(TypeName::UINT))
                R=static_cast<unsigned int>(R);
            else
                R=static_cast<int>(R);
            res.back().append(std::to_string(R));
        }
        else if (eval.type() == typeid(bool))
        {
            auto R=std::any_cast<bool>(eval);
            if (R == true)
                res.back().append("true");
            else
                res.back().append("false");
        }
        else if (eval.type() == typeid(char))
        {
            auto R=std::any_cast<char>(eval);
            if (R <= 31)
                res.back().append(std::to_string(R));
            else if (R==32)
                res.back().append("(space)");
            else
                res.back().push_back(R);
        }
    }
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

Type Scope::getType(const std::string& name)
{
    if (table.contains(name))
        return Type(TypeName::ILLEGAL);
    return *table[name].first;
}
std::any Scope::getEval(const std::string& name)
{
    if (table.contains(name))
        return std::any();
    return table[name].second;
}
void Scope::set(const std::string& name, Type* T, const std::any& Eval)
{
    table[name] = std::make_pair(T, Eval);
}
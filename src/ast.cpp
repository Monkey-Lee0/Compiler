#include "ast.h"
#include<iostream>

std::string showAny(const Type& realType, const std::any& eval)
{
    if (eval.type() == typeid(long long))
    {
        auto R=std::any_cast<long long>(eval);
        if (realType == Type(TypeName::U32) || realType == Type(TypeName::USIZE) ||
            realType == Type(TypeName::UINT))
            R=static_cast<unsigned int>(R);
        else
            R=static_cast<int>(R);
        return std::to_string(R);
    }
    if (eval.type() == typeid(bool))
    {
        auto R=std::any_cast<bool>(eval);
        return R == true ? "true" : "false";
    }
    if (eval.type() == typeid(char))
    {
        auto R=std::any_cast<char>(eval);
        if (R <= 31)
            return std::to_string(R);
        if (R==32)
            return "(space)";
        std::string A;A.push_back(R);
        return A;
    }
    if (eval.type() == typeid(std::vector<std::any>))
    {
        auto R=std::any_cast<std::vector<std::any>>(eval);
        std::string res="[";
        for (int i=0; i<R.size(); i++)
        {
            res.append(showAny(*realType.typePtr,R[i]));
            if (i+1 != R.size())
                res.push_back(',');
        }
        res.push_back(']');
        return res;
    }
    return "";
}

std::vector<std::string> astNode::showSelf() const
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
        case astNodeType::TYPED_IDENTIFIER:
            res.back() = "typed identifier";
            break;
        case astNodeType::ENUM:
            res.back() = "enum";
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
    if (eval.has_value() && showAny(realType, eval).size() <= 50)
    {
        res.back().append("   eval:");
        res.back().append(showAny(realType, eval));
    }
    if (isMutable)
        res.back().append(" mutable");
    if (variableID)
        res.back().append(" %"+std::to_string(variableID));
    if (autoDerefCount)
        res.back().append(" autoDerefCount:"+std::to_string(autoDerefCount));
    if (!irResult.empty() && irResult.size() <= 50)
        res.back().append(" irResult:"+irResult);
    return res;
}

std::vector<std::string> astNode::showTree() const
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

scopeInfo C_ILLEGAL = {Type(TypeName::ILLEGAL), std::any(), false, false, 0};

scopeInfo& Scope::getType(const std::string& name)
{
    if (!typeTable.contains(name))
        return C_ILLEGAL;
    return typeTable[name];
}
scopeInfo& Scope::getItem(const std::string& name)
{
    if (!itemTable.contains(name))
        return C_ILLEGAL;
    return itemTable[name];
}
void Scope::setType(const std::string& name, const scopeInfo &value){typeTable[name] = value;}
void Scope::setItem(const std::string& name, const scopeInfo &value){itemTable[name] = value;}
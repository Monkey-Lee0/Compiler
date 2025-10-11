#include "ir.h"
#include<iostream>

std::string opeToIrString(const std::string &ope)
{
    if (ope == "+")
        return "add";
    if (ope == "-")
        return "sub";
    if (ope == "*")
        return "mul";
    if (ope == "/")
        return "sdiv";
    if (ope == "%")
        return "srem";
    if (ope == "<<")
        return "shl";
    if (ope == ">>")
        return "ashr";
    if (ope == "&" || ope == "&&")
        return "and";
    if (ope == "^")
        return "xor";
    if (ope == "|" || ope == "||")
        return "or";
    return "illegal";
}

std::string typeToIrString(const Type &type)
{
    if (type == I32 || type == U32 || type == ISIZE || type == USIZE || type == IINT || type == UINT || type == INT)
        return "i32";
    if (type == CHAR)
        return "i8";
    if (type == BOOL)
        return "i1";
    if (type == UNIT)
        return "void";
    //TODO
    //other type & array
    return "illegalType";
}

void updateIrState(astNode* node)
{
    if (node->eval.has_value())
    {
        if (node->eval.type() == typeid(long long))
            node->irResult = std::to_string(static_cast<unsigned int>(
                std::any_cast<long long>(node->eval)));
        else if (node->eval.type() == typeid(bool))
            node->irResult = std::any_cast<bool>(node->eval) ? "1" : "0";
        else if (node->eval.type() == typeid(char))
            node->irResult = std::to_string(static_cast<unsigned int>(
                static_cast<unsigned char>(std::any_cast<char>(node->eval))));
        else if (node->eval.type() == typeid(UNIT))
            node->irResult = "()";
        // TODO
        // array?
        return ;
    }
    for (const auto child:node->children)
        updateIrState(child);

    if (node->type == astNodeType::PROGRAM)
    {
        for (const auto child:node->children)
            node->irCode.emplace_back(&child->irCode);
    }
    else if (node->type == astNodeType::FUNCTION)
    {
        node->irResult = "define " + node->children[1]->irResult + " @" +
            std::to_string(findScopeType(node->scope, node->value).ID) + "(" +
            node->children[0]->irResult + ")";
        node->irCode.emplace_back(node->irResult);
        node->irCode.emplace_back(std::string("{"));
        node->irCode.emplace_back(&node->children[0]->irCode);
        node->irCode.emplace_back(&node->children[2]->irCode);
        node->irCode.emplace_back(std::string("}"));
    }
    else if (node->type == astNodeType::STATEMENT_BLOCK)
    {
        for (auto child:node->children)
            node->irCode.emplace_back(&child->irCode);
    }
    else if (node->type == astNodeType::EXPRESSION_STATEMENT)
    {
        node->irCode.emplace_back(&node->children[0]->irCode);
    }
    else if (node->type == astNodeType::TYPE)
    {
        node->irResult = typeToIrString(node->realType);
    }
    else if (node->type == astNodeType::TYPED_IDENTIFIER)
    {
        node->irResult = "%"+std::to_string(variableNum++);
        // node->irResult = node->children[0]->irResult + " %" +
        //     std::to_string(findScopeItem(node->father->father->scope, node->value).ID);
    }
    else if (node->type == astNodeType::PARAMETERS)
    {
        std::vector<std::string> store;
        for (int i=0; i<node->children.size(); i++)
        {
            if (i)
                node->irResult.append(", ");
            //TODO
            //ref & deref
            auto res=findScopeItem(node->father->scope, node->children[i]->value);
            node->irResult.append(typeToIrString(res.type)+" "+node->children[i]->irResult);
            node->irCode.emplace_back("%"+std::to_string(res.ID)+" = alloca "+typeToIrString(res.type));
            store.emplace_back("store "+typeToIrString(res.type)+" "+node->children[i]->irResult+
                ", ptr %"+std::to_string(res.ID));
        }
        for (const auto& str:store)
            node->irCode.emplace_back(str);
    }
    else if (node->type == astNodeType::BINARY_OPERATOR)
    {
        node->irCode.emplace_back(&node->children[0]->irCode);
        node->irCode.emplace_back(&node->children[1]->irCode);
        if (node->value == "=")
        {
            node->irResult = "()";
            node->irCode.emplace_back("store "+typeToIrString(node->children[1]->realType)+" "+
                node->children[1]->irResult+", ptr "+node->children[0]->irResult);
        }
        else
        {
            node->irResult = "%"+std::to_string(variableNum++);
            node->irCode.emplace_back(node->irResult+" = "+opeToIrString(node->value)+" "+
                typeToIrString(node->realType)+" "+node->children[0]->irResult+", "+node->children[1]->irResult);
        }
    }
    else if (node->type == astNodeType::IDENTIFIER)
    {
        if (node->realType.name != TypeName::TYPE)
        {
            auto res=findScopeItem(node->scope, node->value);
            if (node->father->value == "=")
            {
                node->irResult = "%"+std::to_string(res.ID);
            }
            else
            {
                node->irResult = "%"+std::to_string(variableNum++);
                node->irCode.emplace_back(node->irResult+" = load "+typeToIrString(res.type)+
                    +", ptr %"+std::to_string(res.ID));
            }
        }
    }
    else if (node->type == astNodeType::RETURN_CUR)
    {
        node->irCode.emplace_back(&node->children[0]->irCode);
    }
}

std::string setDent(const int &num)
{
    std::string res;
    for (int i=0;i<num;i++)
        res.push_back(' ');
    return res;
}

std::vector<std::string> integrateCode(const std::vector<std::any> &code, int dent=0)
{
    std::vector<std::string> res;
    for (auto ele: code)
    {
        if (ele.type() == typeid(std::string))
        {
            auto str=std::any_cast<std::string>(ele);
            if (str == "}")
                dent -= 2;
            res.push_back(setDent(dent)+str);
            if (str == "{")
                dent += 2;
        }
        else
        {
            auto extend = integrateCode(*std::any_cast<std::vector<std::any>*>(ele));
            for (const auto& str:extend)
                res.push_back(setDent(dent)+str);
        }
    }
    return res;
}

std::vector<std::string> generateIr(astNode* node)
{
    updateIrState(node);
    return integrateCode(node->irCode);
}
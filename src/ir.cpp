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

std::string typeToIrString(Type type)
{
    if (type.name == TypeName::TYPE)
        type = typeToItem(type);
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
            std::to_string(node->variableID) + "(" +
            node->children[0]->irResult + ")";
        node->irCode.emplace_back(node->irResult);
        node->irCode.emplace_back(std::string("{"));
        node->irCode.emplace_back(&node->children[0]->irCode);
        node->irCode.emplace_back(&node->children[2]->irCode);
        if (node->children[2]->realType != NEVER)
            node->irCode.emplace_back(std::string("ret void"));
        node->irCode.emplace_back(std::string("}"));
    }
    else if (node->type == astNodeType::STATEMENT_BLOCK)
    {
        for (auto child:node->children)
        {
            node->irCode.emplace_back(&child->irCode);
            if (child->realType == NEVER)
                break;
        }
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
        node->irResult = typeToIrString(node->realType)+" %"+std::to_string(++variableNum);
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
            node->irResult.append(node->children[i]->irResult);
            node->irCode.emplace_back("%"+std::to_string(node->children[i]->variableID)+" = alloca "+
                typeToIrString(node->children[i]->realType));
            store.emplace_back("store "+node->children[i]->irResult+", ptr %"+
                std::to_string(node->children[i]->variableID));
        }
        for (const auto& str:store)
            node->irCode.emplace_back(str);
    }
    else if (node->type == astNodeType::IF)
    {
        if (node->children.size() == 2)
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            if (node->children[0]->eval.has_value())
            {
                auto cond=std::any_cast<bool>(node->children[0]->eval);
                if (cond)
                    node->irCode.emplace_back(&node->children[1]->irCode);
            }
            else
            {
                node->irCode.emplace_back("dent out");
                // node->irC
            }
        }
    }
    else if (node->type == astNodeType::LET_STATEMENT)
    {
        if (node->children[2]->realType != UNIT)
        {
            node->irCode.emplace_back(&node->children[2]->irCode);
            node->irCode.emplace_back("%"+std::to_string(node->variableID)+" = alloca "+
                typeToIrString(node->realType));
            node->irCode.emplace_back("store "+typeToIrString(node->realType)+" "+
                node->children[2]->irResult+", ptr %"+std::to_string(node->variableID));
        }
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
            node->irResult = "%"+std::to_string(node->variableID);
            node->irCode.emplace_back(node->irResult+" = "+opeToIrString(node->value)+" "+
                typeToIrString(node->realType)+" "+node->children[0]->irResult+", "+node->children[1]->irResult);
        }
    }
    else if (node->type == astNodeType::RETURN)
    {
        if (node->children.empty())
            node->irCode.emplace_back(std::string("ret void"));
        else
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            if (node->children[0]->realType != NEVER)
                node->irCode.emplace_back("ret "+typeToIrString(node->children[0]->realType)+" "+
                    node->children[0]->irResult);
        }
    }
    else if (node->type == astNodeType::GROUP_EXPRESSION)
    {
        for (int i=0;i<node->children.size();i++)
        {
            if (i)
                node->irResult += ", ";
            node->irResult += typeToIrString(node->children[i]->realType)+" "+node->children[i]->irResult;
        }
    }
    else if (node->type == astNodeType::FUNCTION_CALL)
    {
        if (node->realType != UNIT)
        {
            node->irCode.emplace_back("%"+std::to_string(node->variableID)+" = call "+
                typeToIrString(node->realType)+" @"+std::to_string(node->children[0]->variableID)+
                "("+node->children[1]->irResult+")");
        }
        else
        {
            node->irCode.emplace_back("call void @"+std::to_string(node->children[0]->variableID)+
                "("+node->children[1]->irResult+")");
        }
    }
    else if (node->type == astNodeType::IDENTIFIER)
    {
        if (node->realType.name != TypeName::TYPE)
        {
            if (node->father->value == "=")
            {
                node->irResult = "%"+std::to_string(node->variableID);
            }
            else
            {
                node->irResult = "%"+std::to_string(++variableNum);
                node->irCode.emplace_back(node->irResult+" = load "+
                    typeToIrString(node->realType)+
                    +", ptr %"+std::to_string(node->variableID));
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
            if (str == "dent in")
                dent += 2;
            else if (str == "dent out")
                dent-=2;
            else
            {
                if (str == "}")
                    dent -= 2;
                res.push_back(setDent(dent)+str);
                if (str == "{")
                    dent += 2;
            }
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
    // return {};
    return integrateCode(node->irCode);
}
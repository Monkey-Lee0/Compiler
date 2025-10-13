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
    if (ope == "&")
        return "and";
    if (ope == "^")
        return "xor";
    if (ope == "|")
        return "or";
    if (ope == "==")
        return "eq";
    if (ope == "!=")
        return "ne";
    if (ope == ">")
        return "sgt";
    if (ope == ">=")
        return "sge";
    if (ope == "<")
        return "slt";
    if (ope == "<=")
        return "sle";
    return "illegal";
}

std::string opeToIrStringU(const std::string &ope)
{
    if (ope == "==")
        return "eq";
    if (ope == "!=")
        return "ne";
    if (ope == ">")
        return "ugt";
    if (ope == ">=")
        return "uge";
    if (ope == "<")
        return "ult";
    if (ope == "<=")
        return "ule";
    return "illegal";
}

std::string typeToIrString(Type type)
{
    if (type.name == TypeName::TYPE)
        type = typeToItem(type);
    if (type == I32 || type == U32 || type == ISIZE || type == USIZE || type == IINT || type == UINT || type == INT ||
        type.name == TypeName::ENUM)
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

    // pre-set some labels and variables.
    if (node->type == astNodeType::LOOP || node->type == astNodeType::WHILE)
    {
        node->irResultLabel1 = "%"+std::to_string(++variableNum);
        node->irResultLabel2 = "%"+std::to_string(++variableNum);
        if (node->type == astNodeType::LOOP)
            node->irResult = "%"+std::to_string(++variableNum);
    }
    if (node->type == astNodeType::FUNCTION)
    {
        node->irResultLabel1 = "%"+std::to_string(++variableNum);
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
        node->irCode.emplace_back(node->irResultLabel1.substr(1)+":");
        node->irCode.emplace_back(std::string("dent in"));
        node->irCode.emplace_back(&node->children[0]->irCode);
        node->irCode.emplace_back(&node->children[2]->irCode);
        if (node->children[2]->realType != NEVER)
            node->irCode.emplace_back(std::string("ret void"));
        node->irCode.emplace_back(std::string("dent out"));
        node->irCode.emplace_back(std::string("}"));
    }
    else if (node->type == astNodeType::STATEMENT_BLOCK)
    {
        for (auto child:node->children)
        {
            node->irCode.emplace_back(&child->irCode);
            if (child->realType == NEVER && (node->realType == UNIT || node->realType == NEVER))
                break;
        }
        if (node->realType == UNIT)
            node->irResult = "()";
        else
            node->irResult = node->children.back()->irResult;
    }
    else if (node->type == astNodeType::EXPRESSION_STATEMENT)
    {
        node->irCode.emplace_back(&node->children[0]->irCode);
        node->irResult = node->children[0]->irResult;
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
        if (node->children.size() == 2) // if-only
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
                node->irCode.emplace_back("br i1 "+node->children[0]->irResult+", label %"+
                    std::to_string(variableNum+1)+", label %"+std::to_string(variableNum+2));

                node->irCode.emplace_back(std::string("empty line"));
                node->irCode.emplace_back(std::string("dent out"));
                node->irCode.emplace_back(std::to_string(variableNum+1)+":");
                node->irCode.emplace_back(std::string("dent in"));
                node->irCode.emplace_back(&node->children[1]->irCode);
                if (node->children[1]->realType != NEVER)
                    node->irCode.emplace_back("br label %"+std::to_string(variableNum+2));

                node->irCode.emplace_back(std::string("empty line"));
                node->irCode.emplace_back(std::string("dent out"));
                node->irCode.emplace_back(std::to_string(variableNum+2)+":");
                node->irCode.emplace_back(std::string("dent in"));
                variableNum+=2;
            }
        }
        else // if-else
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            if (node->children[0]->eval.has_value())
            {
                auto cond=std::any_cast<bool>(node->children[0]->eval);
                if (cond)
                    node->irCode.emplace_back(&node->children[1]->irCode);
                else
                    node->irCode.emplace_back(&node->children[2]->irCode);
            }
            else
            {
                node->irCode.emplace_back("br i1 "+node->children[0]->irResult+", label %"+
                    std::to_string(variableNum+1)+", label %"+std::to_string(variableNum+2));

                node->irCode.emplace_back(std::string("empty line"));
                node->irCode.emplace_back(std::string("dent out"));
                node->irCode.emplace_back(std::to_string(variableNum+1)+":");
                node->irCode.emplace_back(std::string("dent in"));
                node->irCode.emplace_back(&node->children[1]->irCode);
                if (node->children[1]->realType != NEVER)
                    node->irCode.emplace_back("br label %"+std::to_string(variableNum+3));

                node->irCode.emplace_back(std::string("empty line"));
                node->irCode.emplace_back(std::string("dent out"));
                node->irCode.emplace_back(std::to_string(variableNum+2)+":");
                node->irCode.emplace_back(std::string("dent in"));
                node->irCode.emplace_back(&node->children[2]->irCode);
                if (node->children[2]->realType != NEVER)
                    node->irCode.emplace_back("br label %"+std::to_string(variableNum+3));

                node->irCode.emplace_back(std::string("empty line"));
                node->irCode.emplace_back(std::string("dent out"));
                node->irCode.emplace_back(std::to_string(variableNum+3)+":");
                node->irCode.emplace_back(std::string("dent in"));
                variableNum+=3;
            }
        }
    }
    else if (node->type == astNodeType::ELSE)
    {
        node->irCode.emplace_back(&node->children[0]->irCode);
    }
    else if (node->type == astNodeType::WHILE)
    {
        if (node->children[0]->eval.has_value())
        {
            auto cond=std::any_cast<bool>(node->children[0]->eval);
            if (cond)
            {
                node->irCode.emplace_back("br label "+node->irResultLabel1);

                node->irCode.emplace_back(std::string("empty line"));
                node->irCode.emplace_back(std::string("dent out"));
                node->irCode.emplace_back(node->irResultLabel1.substr(1)+":");
                node->irCode.emplace_back(std::string("dent in"));
                node->irCode.emplace_back(&node->children[1]->irCode);
                if (node->children[1]->realType != NEVER)
                    node->irCode.emplace_back("br label "+node->irResultLabel1);

                node->irCode.emplace_back(std::string("empty line"));
                node->irCode.emplace_back(std::string("dent out"));
                node->irCode.emplace_back(node->irResultLabel2.substr(1)+":");
                node->irCode.emplace_back(std::string("dent in"));
            }
        }
        else
        {
            node->irCode.emplace_back("br label "+node->irResultLabel1);

            node->irCode.emplace_back(std::string("empty line"));
            node->irCode.emplace_back(std::string("dent out"));
            node->irCode.emplace_back(node->irResultLabel1.substr(1)+":");
            node->irCode.emplace_back(std::string("dent in"));
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irCode.emplace_back("br i1 "+node->children[0]->irResult+", label %"+
                    std::to_string(variableNum+1)+", label "+node->irResultLabel2);

            node->irCode.emplace_back(std::string("empty line"));
            node->irCode.emplace_back(std::string("dent out"));
            node->irCode.emplace_back(std::to_string(variableNum+1)+":");
            node->irCode.emplace_back(std::string("dent in"));
            node->irCode.emplace_back(&node->children[1]->irCode);
            if (node->children[1]->realType != NEVER)
                node->irCode.emplace_back("br label "+node->irResultLabel1);

            node->irCode.emplace_back(std::string("empty line"));
            node->irCode.emplace_back(std::string("dent out"));
            node->irCode.emplace_back(node->irResultLabel2.substr(1)+":");
            node->irCode.emplace_back(std::string("dent in"));
            variableNum+=1;
        }
    }
    else if (node->type == astNodeType::LOOP)
    {
        if (node->realType == UNIT)
            node->irResult = "()";
        else
            node->irCode.emplace_back(node->irResult+" = alloca "+typeToIrString(node->realType));
        node->irCode.emplace_back("br label "+node->irResultLabel1);

        node->irCode.emplace_back(std::string("empty line"));
        node->irCode.emplace_back(std::string("dent out"));
        node->irCode.emplace_back(node->irResultLabel1.substr(1)+":");
        node->irCode.emplace_back(std::string("dent in"));
        node->irCode.emplace_back(&node->children[0]->irCode);
        if (node->children[0]->realType != NEVER)
            node->irCode.emplace_back("br label "+node->irResultLabel1);

        node->irCode.emplace_back(std::string("empty line"));
        node->irCode.emplace_back(std::string("dent out"));
        node->irCode.emplace_back(node->irResultLabel2.substr(1)+":");
        node->irCode.emplace_back(std::string("dent in"));
    }
    else if (node->type == astNodeType::CONTINUE)
    {
        node->irCode.emplace_back("br label "+node->loopPtr->irResultLabel1);
    }
    else if (node->type == astNodeType::BREAK)
    {
        if (node->children.empty())
            node->irCode.emplace_back("br label "+node->loopPtr->irResultLabel2);
        else
        {
            node->irCode.emplace_back("store "+typeToIrString(node->children[0]->realType)+
                " "+node->children[0]->irResult+", ptr "+node->loopPtr->irResult);
            node->irCode.emplace_back("br label "+node->loopPtr->irResultLabel2);
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
        if (node->value == "=")
        {
        node->irCode.emplace_back(&node->children[0]->irCode);
        node->irCode.emplace_back(&node->children[1]->irCode);
            node->irResult = "()";
            node->irCode.emplace_back("store "+typeToIrString(node->children[0]->realType)+" "+
                node->children[1]->irResult+", ptr "+node->children[0]->irResultPtr);
        }
        else if (node->value == "+=" || node->value == "-=" || node->value == "*=" ||node->value == "/=" ||
                node->value == "%=" ||node->value == "<<=" || node->value == ">>=" || node->value =="&=" ||
                node->value == "^=" || node->value == "|=")
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irCode.emplace_back(&node->children[1]->irCode);
            node->irResult = "()";
            node->irCode.emplace_back("%"+std::to_string(++variableNum)+" = "+
                opeToIrString(node->value.substr(0,node->value.size()-1))+" "+
                typeToIrString(node->children[0]->realType)+" "+node->children[0]->irResult+", "+node->children[1]->irResult);
            node->irCode.emplace_back("store "+typeToIrString(node->children[1]->realType)+" %"+
                std::to_string(variableNum)+", ptr "+node->children[0]->irResultPtr);
        }
        else if (node->value == "||")
        {
            node->irResultLabel1="%"+std::to_string(++variableNum);
            node->irResultLabel2="%"+std::to_string(++variableNum);
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irCode.emplace_back("br i1 "+node->children[0]->irResult+", label "+node->irResultLabel2+
                ", label"+node->irResultLabel1);

            node->irCode.emplace_back(std::string("empty line"));
            node->irCode.emplace_back(std::string("dent out"));
            node->irCode.emplace_back(node->irResultLabel1.substr(1)+":");
            node->irCode.emplace_back(std::string("dent in"));
            node->irCode.emplace_back(&node->children[1]->irCode);

            node->irCode.emplace_back(std::string("empty line"));
            node->irCode.emplace_back(std::string("dent out"));
            node->irCode.emplace_back(node->irResultLabel2.substr(1)+":");
            node->irCode.emplace_back(std::string("dent in"));

            node->irResult = "%"+std::to_string(++variableNum);
            node->irCode.emplace_back(node->irResult+" = phi i1 [ "+node->children[0]->irResult+", "+
                node->fnPtr->irResultLabel1+" ], [ "+node->children[1]->irResult+", "+
                node->irResultLabel1+" ]");
        }
        else if (node->value == "&&")
        {
            node->irResultLabel1="%"+std::to_string(++variableNum);
            node->irResultLabel2="%"+std::to_string(++variableNum);
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irCode.emplace_back("br i1 "+node->children[0]->irResult+", label "+node->irResultLabel1+
                ", label"+node->irResultLabel2);

            node->irCode.emplace_back(std::string("empty line"));
            node->irCode.emplace_back(std::string("dent out"));
            node->irCode.emplace_back(node->irResultLabel1.substr(1)+":");
            node->irCode.emplace_back(std::string("dent in"));
            node->irCode.emplace_back(&node->children[1]->irCode);

            node->irCode.emplace_back(std::string("empty line"));
            node->irCode.emplace_back(std::string("dent out"));
            node->irCode.emplace_back(node->irResultLabel2.substr(1)+":");
            node->irCode.emplace_back(std::string("dent in"));

            node->irResult = "%"+std::to_string(++variableNum);
            node->irCode.emplace_back(node->irResult+" = phi i1 [ "+node->children[0]->irResult+", "+
                node->fnPtr->irResultLabel1+" ], [ "+node->children[1]->irResult+", "+
                node->irResultLabel1+" ]");
        }
        else if (node->value == "==" || node->value == "!=" || node->value == "<" ||
            node->value == "<=" || node->value == ">" || node->value == ">=")
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irCode.emplace_back(&node->children[1]->irCode);
            node->irResult = "%"+std::to_string(++variableNum);
            if (node->children[0]->realType == U32 || node->children[0]->realType == USIZE ||
                node->children[0]->realType == UINT)
                node->irCode.emplace_back(node->irResult+" = icmp "+opeToIrStringU(node->value)+" "+
                    typeToIrString(node->children[0]->realType)+" "+node->children[0]->irResult+", "+
                    node->children[1]->irResult);
            else
                node->irCode.emplace_back(node->irResult+" = icmp "+opeToIrString(node->value)+" "+
                    typeToIrString(node->children[0]->realType)+" "+node->children[0]->irResult+", "+
                    node->children[1]->irResult);
        }
        else
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irCode.emplace_back(&node->children[1]->irCode);
            node->irResult = "%"+std::to_string(node->variableID);
            node->irCode.emplace_back(node->irResult+" = "+opeToIrString(node->value)+" "+
                typeToIrString(node->realType)+" "+node->children[0]->irResult+", "+node->children[1]->irResult);
        }
    }
    else if (node->type == astNodeType::UNARY_OPERATOR)
    {
        if (node->value == "-")
        {
            node->irResult = "%"+std::to_string(++variableNum);
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irCode.emplace_back(node->irResult+" = sub "+typeToIrString(node->realType)+" 0, "+
                node->children[0]->irResult);
        }
        else if (node->value == "!")
        {
            node->irResult = "%"+std::to_string(++variableNum);
            node->irCode.emplace_back(&node->children[0]->irCode);
            if (node->realType == BOOL)
                node->irCode.emplace_back(node->irResult+" = xor "+typeToIrString(node->realType)+" 1, "+
                    node->children[0]->irResult);
            else
                node->irCode.emplace_back(node->irResult+" = xor "+typeToIrString(node->realType)+" -1, "+
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
            node->irResult = "%"+std::to_string(++variableNum);
            node->irCode.emplace_back(node->irResult+" = load "+
                typeToIrString(node->realType)+
                +", ptr %"+std::to_string(node->variableID));
            node->irResultPtr = "%"+std::to_string(node->variableID);
        }
    }
    else if (node->type == astNodeType::RETURN_CUR)
    {
        node->irCode.emplace_back(&node->children[0]->irCode);
        node->irResult = node->children[0]->irResult;
    }
}

std::string setDent(const int &num)
{
    std::string res;
    for (int i=0;i<num;i++)
        res.push_back(' ');
    return res;
}

std::vector<std::string> integrateCode(const std::vector<std::any> &code, int &dent)
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
                dent -= 2;
            else if (str == "empty line")
                res.emplace_back("");
            else
                res.push_back(setDent(dent)+str);
        }
        else
        {
            auto extend = integrateCode(*std::any_cast<std::vector<std::any>*>(ele), dent);
            for (const auto& str:extend)
                res.push_back(str);
        }
    }
    return res;
}

std::vector<std::string> trimString(const std::vector<std::string> &code)
{
    std::vector<std::string> res;
    int lasLabel=0;
    for (int i=0;i<code.size();i++)
    {
        if (code[i].empty())
            continue;
        if (code[i] == "}")
            res.push_back(code[i]), lasLabel = i+1;
        else if (code[i].back() == ':')
            lasLabel = i;
        else
            while (lasLabel<=i)
                res.push_back(code[lasLabel]), lasLabel++;
    }
    return res;
}

std::vector<std::string> generateIr(astNode* node)
{
    updateIrState(node);
    int dent = 0;
    return trimString(integrateCode(node->irCode, dent));
}
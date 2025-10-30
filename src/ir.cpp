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
    if (type.name == TypeName::REF || type.name == TypeName::MUT_REF || type == STR || type == STRING)
        return "ptr";
    if (type.name == TypeName::ARRAY)
        return "["+std::to_string(type.len)+" x "+typeToIrString(*type.typePtr)+"]";
    if (type.name == TypeName::STRUCT)
        return "%v"+std::to_string(type.structID);
    return "illegalType";
}

std::string typeToIrStringPtr(Type type)
{
    if (type.name == TypeName::TYPE)
        type = typeToItem(type);
    if (type.name == TypeName::ARRAY || type.name == TypeName::STRUCT)
        return "ptr";
    return typeToIrString(type);
}

std::string constantToIrString(const std::any &eval,const Type& typ)
{
    if (eval.type() == typeid(long long))
        return std::to_string(static_cast<unsigned int>(std::any_cast<long long>(eval)));
    if (eval.type() == typeid(bool))
        return std::any_cast<bool>(eval) ? "1" : "0";
    if (eval.type() == typeid(char))
        return std::to_string(static_cast<unsigned int>(static_cast<unsigned char>(std::any_cast<char>(eval))));
    if (eval.type() == typeid(UNIT))
        return "()";
    if (eval.type() == typeid(std::vector<std::any>))
    {
        std::string res="[";
        auto arr = std::any_cast<std::vector<std::any>>(eval);
        for (int i=0;i<arr.size();i++)
        {
            if (i)
                res+=", ";
            res+=typeToIrString(*typ.typePtr);
            res+=" ";
            res+=constantToIrString(arr[i], *typ.typePtr);
        }
        res+="]";
        return res;
    }
}

bool isBigType(Type type){return type.name == TypeName::ARRAY || type.name == TypeName::STRUCT;}

void llvmMemcpyCaller(std::vector<std::any> &code, std::string dest, std::string src, Type type)
{
    auto sizePtr = "%c"+std::to_string(++variableNum);
    auto sizeVar = "%c"+std::to_string(++variableNum);
    code.emplace_back(sizePtr+" = getelementptr "+typeToIrString(type)+
        ", ptr null, i32 1");
    code.emplace_back(sizeVar+" = ptrtoint ptr "+sizePtr+" to i32");
    code.emplace_back("call void @llvm.memcpy.p0.p0.i32(ptr "+dest+
        ", ptr "+src+", i32 "+sizeVar+", i1 false)");
}

void llvmStore(std::vector<std::any> &code, std::string dest, std::string src, Type type)
{
    if (isBigType(type))
        llvmMemcpyCaller(code, dest, src, type);
    else
        code.emplace_back("store "+typeToIrString(type)+" "+src+", ptr "+dest);
}

void llvmLoad(std::vector<std::any> &code, std::string dest, std::string src, Type type)
{
    if (isBigType(type))
        llvmMemcpyCaller(code, dest, src, type);
    else
        code.emplace_back(dest+" = load "+typeToIrString(type)+", ptr "+src);
}

void updateIrState(astNode* node, astNode* root)
{
    if (node->eval.has_value() && node->eval.type() != typeid(std::vector<std::any>))
    {
        if (node->eval.type() == typeid(std::string))
        {
            node->irResult = "@s"+std::to_string(++variableNum);
            auto value = std::any_cast<std::string>(node->eval);
            root->irCodeStruct.emplace_back(node->irResult+" = global ["+std::to_string(value.size()+1)+" x i8] c\""+
               value+"\\00\"");
        }
        else
            node->irResult = constantToIrString(node->eval, node->realType);
        return ;
    }

    // pre-set some labels and variables.
    if (node->type == astNodeType::LOOP || node->type == astNodeType::WHILE)
    {
        node->irResultLabel1 = "%v"+std::to_string(++variableNum);
        node->irResultLabel2 = "%v"+std::to_string(++variableNum);
        if (node->type == astNodeType::LOOP)
        {
            node->irResultPtr = "%v"+std::to_string(++variableNum);
            node->irResult = "%v"+std::to_string(++variableNum);
        }
    }
    if (node->type == astNodeType::FUNCTION)
    {
        node->irResultLabel1 = "%v"+std::to_string(++variableNum);
        node->irResultPtr = "%v"+std::to_string(++variableNum);
    }

    for (const auto child:node->children)
        updateIrState(child, root);

    if (node->type == astNodeType::PROGRAM)
    {
        for (const auto child:node->children)
            if(child->type == astNodeType::STRUCT)
                root->irCodeStruct.emplace_back(&child->irCode);
            else
                root->irCode.emplace_back(&child->irCode);
    }
    else if (node->type == astNodeType::STRUCT)
    {
        const auto realT=typeToItem(node->realType);
        node->irResult = "%v"+std::to_string(realT.structID);
        if (realT.members.empty())
            node->irCode.emplace_back(node->irResult+" = type { i8 }");
        else
        {
            std::string res=node->irResult+" = type { ";
            for (int i=0; i<realT.members.size(); i++)
            {
                if (i)
                    res+=", ";
                res+=typeToIrString(*realT.members[i]);
            }
            res+=" }";
            node->irCode.emplace_back(res);
        }
    }
    else if (node->type == astNodeType::FUNCTION)
    {
        if (node->variableID == 1)
            node->irResult = "define " + node->children[1]->irResult + " @main("+node->children[0]->irResult + ")";
        else if (node->variableID == 2)
            node->irResult = "define " + node->children[1]->irResult + " @exit("+node->children[0]->irResult + ")";
        else if (node->variableID == 3)
            node->irResult = "define " + node->children[1]->irResult + " @printInt("+node->children[0]->irResult + ")";
        else if (node->variableID == 4)
            node->irResult = "define " + node->children[1]->irResult + " @printlnInt("+node->children[0]->irResult + ")";
        else if (node->variableID == 5)
            node->irResult = "define " + node->children[1]->irResult + " @getInt("+node->children[0]->irResult + ")";
        else
        {
            if (isBigType(typeToItem(node->children[1]->realType)))
            {
                if (!node->children[0]->irResult.empty())
                    node->children[0]->irResult += ", ";
                node->children[0]->irResult += "ptr "+node->irResultPtr;
                node->irResult = "define void @f" + std::to_string(node->variableID) +
                    "(" + node->children[0]->irResult + ")";
            }
            else
                node->irResult = "define " + typeToIrString(node->children[1]->realType) + " @f" +
                    std::to_string(node->variableID) + "(" + node->children[0]->irResult + ")";
        }
        node->irCode.emplace_back(node->irResult);
        node->irCode.emplace_back(std::string("{"));
        node->irCode.emplace_back(node->irResultLabel1.substr(1)+":");
        node->irCode.emplace_back(std::string("dent in"));
        node->irCode.emplace_back(&node->children[0]->irCode);
        node->irCode.emplace_back(&node->children[2]->irCode);

        auto T=node->children[2]->realType;
        if (T != NEVER)
        {
            if (T == UNIT)
                node->irCode.emplace_back(std::string("ret void"));
            else if (isBigType(T))
            {
                llvmStore(node->irCode, node->irResultPtr,
                    node->children[2]->irResult, node->children[2]->realType);
                node->irCode.emplace_back(std::string("ret void"));
            }
            else
                node->irCode.emplace_back("ret "+typeToIrStringPtr(T)+" "+node->children[2]->irResult);
        }
        node->irCode.emplace_back(std::string("dent out"));
        node->irCode.emplace_back(std::string("}"));
        node->irCode.emplace_back(std::string("empty line"));
    }
    else if (node->type == astNodeType::IMPL)
    {
        for (auto child:node->children[0]->children)
            if(child->type == astNodeType::STRUCT)
                root->irCodeStruct.emplace_back(&child->irCode);
            else
                root->irCode.emplace_back(&child->irCode);
    }
    else if (node->type == astNodeType::STATEMENT_BLOCK)
    {
        for (auto child:node->children)
        {
            if (child->type == astNodeType::FUNCTION)
                root->irCode.emplace_back(&child->irCode);
            else if (child->type == astNodeType::STRUCT)
                root->irCodeStruct.emplace_back(&child->irCode);
            else
                node->irCode.emplace_back(&child->irCode);
            if (child->realType == NEVER)
            {
                node->realType = NEVER;
                break;
            }
        }
        if (node->realType == UNIT || node->realType == NEVER)
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
        node->irResult = "%v"+std::to_string(++variableNum);
    }
    else if (node->type == astNodeType::PARAMETERS)
    {
        std::vector<std::any> store;
        for (int i=0; i<node->children.size(); i++)
        {
            if (i)
                node->irResult.append(", ");
            node->irResult.append(typeToIrStringPtr(node->children[i]->realType)+" "+
                node->children[i]->irResult);
            node->irCode.emplace_back("%v"+std::to_string(node->children[i]->variableID)+" = alloca "+
                typeToIrString(node->children[i]->realType));
            llvmStore(store, "%v"+std::to_string(node->children[i]->variableID),
                node->children[i]->irResult, node->children[i]->realType);
        }
        for (const auto& str:store)
            node->irCode.emplace_back(str);
    }
    else if (node->type == astNodeType::IF)
    {
        if (node->children.size() == 2) // if-only
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irResult = "()";
            if (node->children[0]->eval.has_value())
            {
                auto cond=std::any_cast<bool>(node->children[0]->eval);
                if (cond)
                    node->irCode.emplace_back(&node->children[1]->irCode);
            }
            else
            {
                auto L1 = variableNum+1, L2 = variableNum+2;
                variableNum += 2;
                node->irCode.emplace_back("br i1 "+node->children[0]->irResult+", label %v"+
                    std::to_string(L1)+", label %v"+std::to_string(L2));

                node->irCode.emplace_back(std::string("empty line"));
                node->irCode.emplace_back(std::string("dent out"));
                node->irCode.emplace_back("v"+std::to_string(L1)+":");
                node->irCode.emplace_back(std::string("dent in"));
                node->irCode.emplace_back(&node->children[1]->irCode);
                if (node->children[1]->realType != NEVER)
                    node->irCode.emplace_back("br label %v"+std::to_string(L2));

                node->irCode.emplace_back(std::string("empty line"));
                node->irCode.emplace_back(std::string("dent out"));
                node->irCode.emplace_back("v"+std::to_string(L2)+":");
                node->irCode.emplace_back(std::string("dent in"));
            }
        }
        else // if-else
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            if (node->children[0]->eval.has_value())
            {
                auto cond=std::any_cast<bool>(node->children[0]->eval);
                if (cond)
                {
                    node->irCode.emplace_back(&node->children[1]->irCode);
                    node->irResult = node->children[1]->irResult;
                }
                else
                {
                    node->irCode.emplace_back(&node->children[2]->irCode);
                    node->irResult = node->children[2]->irResult;
                }
            }
            else
            {
                if (node->realType == UNIT || node->realType == NEVER)
                    node->irResult = "()";
                else
                {
                    node->irResultPtr = "%v"+std::to_string(++variableNum);
                    node->irCode.emplace_back(node->irResultPtr+" = alloca "+typeToIrString(node->realType));
                }

                auto L1 = variableNum+1, L2 = variableNum+2,
                    L3 = variableNum+3;
                variableNum += 3;

                node->irCode.emplace_back("br i1 "+node->children[0]->irResult+", label %v"+
                    std::to_string(L1)+", label %v"+std::to_string(L2));

                node->irCode.emplace_back(std::string("empty line"));
                node->irCode.emplace_back(std::string("dent out"));
                node->irCode.emplace_back("v"+std::to_string(L1)+":");
                node->irCode.emplace_back(std::string("dent in"));
                node->irCode.emplace_back(&node->children[1]->irCode);
                if (node->children[1]->realType != UNIT && node->children[1]->realType != NEVER)
                    llvmStore(node->irCode, node->irResultPtr, node->children[1]->irResult,
                        node->realType);
                if (node->children[1]->realType != NEVER)
                    node->irCode.emplace_back("br label %v"+std::to_string(L3));

                node->irCode.emplace_back(std::string("empty line"));
                node->irCode.emplace_back(std::string("dent out"));
                node->irCode.emplace_back("v"+std::to_string(L2)+":");
                node->irCode.emplace_back(std::string("dent in"));
                node->irCode.emplace_back(&node->children[2]->irCode);
                if (node->children[2]->realType != UNIT && node->children[2]->realType != NEVER)
                    llvmStore(node->irCode, node->irResultPtr,
                        node->children[2]->irResult, node->realType);
                if (node->children[2]->realType != NEVER)
                    node->irCode.emplace_back("br label %v"+std::to_string(L3));

                node->irCode.emplace_back(std::string("empty line"));
                node->irCode.emplace_back(std::string("dent out"));
                node->irCode.emplace_back("v"+std::to_string(L3)+":");
                node->irCode.emplace_back(std::string("dent in"));

                if (node->realType != UNIT && node->realType != NEVER)
                {
                    if (isBigType(node->realType))
                        node->irResult = node->irResultPtr;
                    else
                    {
                        node->irResult = "%v"+std::to_string(++variableNum);
                        llvmLoad(node->irCode, node->irResult, node->irResultPtr, node->realType);
                    }
                }
            }
        }
    }
    else if (node->type == astNodeType::ELSE)
    {
        node->irCode.emplace_back(&node->children[0]->irCode);
        node->irResult = node->children[0]->irResult;
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
            auto L1 = variableNum+1;
            variableNum += 1;
            node->irCode.emplace_back("br label "+node->irResultLabel1);

            node->irCode.emplace_back(std::string("empty line"));
            node->irCode.emplace_back(std::string("dent out"));
            node->irCode.emplace_back(node->irResultLabel1.substr(1)+":");
            node->irCode.emplace_back(std::string("dent in"));
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irCode.emplace_back("br i1 "+node->children[0]->irResult+", label %v"+
                    std::to_string(L1)+", label "+node->irResultLabel2);

            node->irCode.emplace_back(std::string("empty line"));
            node->irCode.emplace_back(std::string("dent out"));
            node->irCode.emplace_back("v"+std::to_string(L1)+":");
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
    else if (node->type == astNodeType::LOOP)
    {
        if (node->realType == UNIT || node->realType == NEVER)
            node->irResult = "()";
        else
            node->irCode.emplace_back(node->irResultPtr+" = alloca "+typeToIrString(node->realType));
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
        if (node->realType != UNIT && node->realType != NEVER)
        {
            if (isBigType(node->realType))
                node->irResult = node->irResultPtr;
            else
                llvmLoad(node->irCode, node->irResult, node->irResultPtr, node->realType);
        }
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
            node->irCode.emplace_back(&node->children[0]->irCode);
            if (node->children[0]->realType != NEVER)
            {
                if (node->children[0]->realType != UNIT)
                    llvmStore(node->irCode, node->loopPtr->irResultPtr, node->children[0]->irResult,
                        node->children[0]->realType);
                node->irCode.emplace_back("br label "+node->loopPtr->irResultLabel2);
            }
        }
    }
    else if (node->type == astNodeType::RETURN)
    {
        if (node->children.empty())
            node->irCode.emplace_back(std::string("ret void"));
        else if (isBigType(node->children[0]->realType))
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            llvmStore(node->irCode, node->fnPtr->irResultPtr,
                node->children[0]->irResult, node->children[0]->realType);
            node->irCode.emplace_back(std::string("ret void"));
        }
        else
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            if (node->children[0]->realType != NEVER)
                node->irCode.emplace_back("ret "+typeToIrStringPtr(node->children[0]->realType)+" "+
                    node->children[0]->irResult);
        }
    }
    else if (node->type == astNodeType::LET_STATEMENT)
    {
        if (node->children[2]->realType == NEVER)
        {
            node->irCode.emplace_back(&node->children[2]->irCode);
            node->realType = NEVER;
        }
        else if (node->children[2]->realType != UNIT)
        {
            node->irCode.emplace_back(&node->children[2]->irCode);
            node->irCode.emplace_back("%v"+std::to_string(node->variableID)+" = alloca "+
                typeToIrString(node->realType));
            llvmStore(node->irCode, "%v"+std::to_string(node->variableID), node->children[2]->irResult,
                node->realType);
        }
    }
    else if (node->type == astNodeType::BINARY_OPERATOR)
    {
        if (node->value == "=")
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irCode.emplace_back(&node->children[1]->irCode);
            node->irResult = "()";
            if (node->children[1]->realType != NEVER && node->children[1]->realType != UNIT)
                llvmStore(node->irCode, node->children[0]->irResultPtr,
                    node->children[1]->irResult, node->children[0]->realType);
        }
        else if (node->value == "+=" || node->value == "-=" || node->value == "*=" ||node->value == "/=" ||
                node->value == "%=" ||node->value == "<<=" || node->value == ">>=" || node->value =="&=" ||
                node->value == "^=" || node->value == "|=")
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irCode.emplace_back(&node->children[1]->irCode);
            node->irResult = "()";
            node->irCode.emplace_back("%v"+std::to_string(++variableNum)+" = "+
                opeToIrString(node->value.substr(0,node->value.size()-1))+" "+
                typeToIrString(node->children[0]->realType)+" "+node->children[0]->irResult+", "+node->children[1]->irResult);
            llvmStore(node->irCode, node->children[0]->irResultPtr, "%v"+std::to_string(variableNum),
                node->children[1]->realType);
        }
        else if (node->value == "||")
        {
            node->irResultLabel1="%v"+std::to_string(++variableNum);
            node->irResultLabel2="%v"+std::to_string(++variableNum);
            node->irResult = "%v"+std::to_string(++variableNum);
            node->irResultPtr = "%v"+std::to_string(++variableNum);

            node->irCode.emplace_back(node->irResultPtr+" = alloca i1");
            llvmStore(node->irCode, node->irResultPtr, "1", BOOL);
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irCode.emplace_back("br i1 "+node->children[0]->irResult+", label "+node->irResultLabel2+
                ", label "+node->irResultLabel1);

            node->irCode.emplace_back(std::string("empty line"));
            node->irCode.emplace_back(std::string("dent out"));
            node->irCode.emplace_back(node->irResultLabel1.substr(1)+":");
            node->irCode.emplace_back(std::string("dent in"));
            node->irCode.emplace_back(&node->children[1]->irCode);
            llvmStore(node->irCode, node->irResultPtr, node->children[1]->irResult, BOOL);
            node->irCode.emplace_back("br label "+node->irResultLabel2);

            node->irCode.emplace_back(std::string("empty line"));
            node->irCode.emplace_back(std::string("dent out"));
            node->irCode.emplace_back(node->irResultLabel2.substr(1)+":");
            node->irCode.emplace_back(std::string("dent in"));

            llvmLoad(node->irCode, node->irResult, node->irResultPtr, BOOL);
        }
        else if (node->value == "&&")
        {
            node->irResultLabel1="%v"+std::to_string(++variableNum);
            node->irResultLabel2="%v"+std::to_string(++variableNum);
            node->irResult = "%v"+std::to_string(++variableNum);
            node->irResultPtr = "%v"+std::to_string(++variableNum);

            node->irCode.emplace_back(node->irResultPtr+" = alloca i1");
            llvmStore(node->irCode, node->irResultPtr, "0", BOOL);
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irCode.emplace_back("br i1 "+node->children[0]->irResult+", label "+node->irResultLabel1+
                ", label "+node->irResultLabel2);

            node->irCode.emplace_back(std::string("empty line"));
            node->irCode.emplace_back(std::string("dent out"));
            node->irCode.emplace_back(node->irResultLabel1.substr(1)+":");
            node->irCode.emplace_back(std::string("dent in"));
            node->irCode.emplace_back(&node->children[1]->irCode);
            llvmStore(node->irCode, node->irResultPtr, node->children[1]->irResult, BOOL);
            node->irCode.emplace_back("br label "+node->irResultLabel2);

            node->irCode.emplace_back(std::string("empty line"));
            node->irCode.emplace_back(std::string("dent out"));
            node->irCode.emplace_back(node->irResultLabel2.substr(1)+":");
            node->irCode.emplace_back(std::string("dent in"));

            llvmLoad(node->irCode, node->irResult, node->irResultPtr, BOOL);
        }
        else if (node->value == "==" || node->value == "!=" || node->value == "<" ||
            node->value == "<=" || node->value == ">" || node->value == ">=")
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irCode.emplace_back(&node->children[1]->irCode);
            node->irResult = "%v"+std::to_string(++variableNum);
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
        else if (node->value == ".")
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            if (node->realType.name == TypeName::FUNCTION)
            {
                node->irResult = "@f"+std::to_string(node->variableID);
                node->selfPtr = node->children[0];
            }
            else
            {
                node->irResultPtr = "%v"+std::to_string(++variableNum);
                auto realT = node->children[0]->realType;
                auto id=realT.memberNames->at(node->children[1]->value);

                node->irCode.emplace_back(node->irResultPtr+" = getelementptr "+typeToIrString(realT)+", ptr "+
                    node->children[0]->irResultPtr+", i32 0, i32 "+std::to_string(id));
                if (isBigType(node->realType))
                    node->irResult = node->irResultPtr;
                else
                {
                    node->irResult = "%v"+std::to_string(++variableNum);
                    llvmLoad(node->irCode, node->irResult, node->irResultPtr, node->realType);
                }
            }
        }
        else if (node->value == "::")
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irResult = "@f"+std::to_string(node->variableID);
        }
        else if (node->value == "as")
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            auto T=node->realType;
            if (node->children[0]->realType == CHAR && (T == INT || T == UINT || T == IINT ||
                T == I32 || T == U32 || T == ISIZE || T == USIZE))
            {
                node->irResult = "%v"+std::to_string(++variableNum);
                node->irCode.emplace_back(node->irResult+" = zext i8 "+node->children[0]->irResult+" to i32");
            }
            else if (node->children[0]->realType == BOOL && (T == INT || T == UINT || T == IINT ||
                T == I32 || T == U32 || T == ISIZE || T == USIZE))
            {
                node->irResult = "%v"+std::to_string(++variableNum);
                node->irCode.emplace_back(node->irResult+" = zext i1 "+node->children[0]->irResult+" to i32");
            }
            else
            {
                node->irResult = node->children[0]->irResult;
                node->irResultPtr = node->children[0]->irResultPtr;
            }
        }
        else
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irCode.emplace_back(&node->children[1]->irCode);
            node->irResult = "%v"+std::to_string(node->variableID);
            node->irCode.emplace_back(node->irResult+" = "+opeToIrString(node->value)+" "+
                typeToIrString(node->realType)+" "+node->children[0]->irResult+", "+node->children[1]->irResult);
        }
    }
    else if (node->type == astNodeType::UNARY_OPERATOR)
    {
        if (node->value == "-")
        {
            node->irResult = "%v"+std::to_string(++variableNum);
            node->irCode.emplace_back(&node->children[0]->irCode);
            node->irCode.emplace_back(node->irResult+" = sub "+typeToIrString(node->realType)+" 0, "+
                node->children[0]->irResult);
        }
        else if (node->value == "!")
        {
            node->irResult = "%v"+std::to_string(++variableNum);
            node->irCode.emplace_back(&node->children[0]->irCode);
            if (node->realType == BOOL)
                node->irCode.emplace_back(node->irResult+" = xor "+typeToIrString(node->realType)+" 1, "+
                    node->children[0]->irResult);
            else
                node->irCode.emplace_back(node->irResult+" = xor "+typeToIrString(node->realType)+" -1, "+
                    node->children[0]->irResult);
        }
        else if (node->value == "&" || node->value == "&mut")
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            if (!isBigType(node->children[0]->realType))
            {
                node->irResult = "%v"+std::to_string(++variableNum);
                node->irCode.emplace_back(node->irResult+" = alloca "+
                    typeToIrString(node->children[0]->realType));
                llvmStore(node->irCode, node->irResult, node->children[0]->irResult,
                    node->children[0]->realType);
            }
            else
                node->irResult = node->children[0]->irResultPtr;
        }
        else if (node->value == "*")
        {
            node->irCode.emplace_back(&node->children[0]->irCode);
            if (!isBigType(node->realType))
            {
                node->irResult = "%v"+std::to_string(++variableNum);
                node->irResultPtr = node->children[0]->irResult;
                llvmLoad(node->irCode, node->irResult, node->irResultPtr, node->realType);
            }
            else
                node->irResult = node->irResultPtr = node->children[0]->irResult;
        }
    }
    else if (node->type == astNodeType::GROUP_EXPRESSION)
    {
        for (int i=0;i<node->children.size();i++)
        {
            if (i)
                node->irResult += ", ";
            node->irCode.emplace_back(&node->children[i]->irCode);
            if (node->father->type == astNodeType::FUNCTION_CALL)
                node->irResult += typeToIrStringPtr(node->children[i]->realType)+" "+
                    node->children[i]->irResult;
            else
                node->irResult += typeToIrString(node->children[i]->realType)+" "+node->children[i]->irResult;
        }
    }
    else if (node->type == astNodeType::FUNCTION_CALL)
    {
        node->irCode.emplace_back(&node->children[0]->irCode);
        node->irCode.emplace_back(&node->children[1]->irCode);
        auto groupResult = node->children[1]->irResult;
        const auto selfPtr = node->children[0]->selfPtr;
        if (selfPtr != nullptr)
        {
            if (!groupResult.empty())
                groupResult = ", "+groupResult;
            groupResult = typeToIrStringPtr(selfPtr->realType)+" "+selfPtr->irResult+groupResult;
        }
        if (isBigType(node->realType))
        {
            node->irResultPtr = node->irResult = "%v"+std::to_string(++variableNum);
            if (!groupResult.empty())
                groupResult += ", ";
            groupResult +="ptr "+node->irResult;
            node->irCode.emplace_back(node->irResult+" = alloca "+typeToIrString(node->realType));
            node->irCode.emplace_back("call void "+node->children[0]->irResult+"("+groupResult+")");
        }
        else
        {
            if (node->children[0]->variableID == 6)
            {
                node->irResult = std::to_string(selfPtr->realType.len);
            }
            else if (node->realType != UNIT)
            {
                node->irResult = "%v"+std::to_string(node->variableID);
                if (isBigType(node->realType))
                    node->irResultPtr = node->irResult;
                node->irCode.emplace_back(node->irResult+" = call "+
                    typeToIrStringPtr(node->realType)+" "+node->children[0]->irResult+"("+groupResult+")");
            }
            else
            {
                node->irResult = "()";
                node->irCode.emplace_back("call void "+node->children[0]->irResult+"("+groupResult+")");
            }
        }

    }
    else if (node->type == astNodeType::IDENTIFIER)
    {
        if (node->realType.name == TypeName::TYPE)
        {
            const auto realT = typeToItem(node->realType);
            if (realT.name == TypeName::STRUCT)
                node->irResult = "%v"+std::to_string(realT.structID);
        }
        else if (node->realType.name == TypeName::FUNCTION)
        {
            if (node->variableID == 1)
                node->irResult = "@main";
            else if (node->variableID == 2)
                node->irResult = "@exit";
            else if (node->variableID == 3)
                node->irResult = "@printInt";
            else if (node->variableID == 4)
                node->irResult = "@printlnInt";
            else if (node->variableID == 5)
                node->irResult = "@getInt";
            else
                node->irResult = "@f"+std::to_string(node->variableID);
        }
        else
        {
            node->irResultPtr = "%v"+std::to_string(node->variableID);
            if (!isBigType(node->realType))
                llvmLoad(node->irCode, node->irResult = "%v"+std::to_string(++variableNum),
                    node->irResultPtr, node->realType);
            else
                node->irResult = node->irResultPtr;
        }
    }
    else if (node->type == astNodeType::RETURN_CUR)
    {
        node->irCode.emplace_back(&node->children[0]->irCode);
        node->irResult = node->children[0]->irResult;
    }
    else if (node->type == astNodeType::ARRAY_BUILD)
    {
        node->irResult = node->irResultPtr = "%v"+std::to_string(++variableNum);
        node->irCode.emplace_back(node->irResultPtr+" = alloca "+typeToIrString(node->realType));
        if (node->children.size() == 1) // [a,b,c]
        {
            for (int i=0; i<node->children[0]->children.size(); i++)
            {
                auto child = node->children[0]->children[i];
                node->irCode.emplace_back(&child->irCode);
                node->irCode.emplace_back("%v"+std::to_string(++variableNum)+" = getelementptr "+
                    typeToIrString(node->realType)+", ptr "+node->irResultPtr+", i32 0, i32 "+
                    std::to_string(i));
                llvmStore(node->irCode, "%v"+std::to_string(variableNum), child->irResult, child->realType);
            }
        }
        else // [a;b]
        {
            auto child = node->children[0];
            node->irCode.emplace_back(&child->irCode);
            auto len = node->realType.len;
            if (len <= 20)
                for (int i=0; i<len; i++) // brute force for small one.
                {
                    node->irCode.emplace_back("%v"+std::to_string(++variableNum)+" = getelementptr "+
                        typeToIrString(node->realType)+", ptr "+node->irResultPtr+", i32 0, i32 "+
                        std::to_string(i));
                    llvmStore(node->irCode, "%v"+std::to_string(variableNum), child->irResult,
                        child->realType);
                }
            else // loop for large one
            {
                auto loopVar = "%v"+std::to_string(++variableNum);
                auto condLabel = "%v"+std::to_string(++variableNum);
                auto blockLabel = "%v"+std::to_string(++variableNum);
                auto endLabel = "%v"+std::to_string(++variableNum);
                node->irCode.emplace_back(loopVar+" = alloca i32");
                llvmStore(node->irCode, loopVar, "0", I32);
                node->irCode.emplace_back("br label "+condLabel);

                node->irCode.emplace_back(std::string("empty line"));
                node->irCode.emplace_back(std::string("dent out"));
                node->irCode.emplace_back(condLabel.substr(1)+":");
                node->irCode.emplace_back(std::string("dent in"));
                auto loadLoopVar = "%v"+std::to_string(++variableNum);
                llvmLoad(node->irCode, loadLoopVar, loopVar, I32);
                auto condVar = "%v"+std::to_string(++variableNum);
                node->irCode.emplace_back(condVar+" = icmp slt i32 "+loadLoopVar+", "+std::to_string(len));
                node->irCode.emplace_back("br i1 "+condVar+", label "+blockLabel+", label "+endLabel);

                node->irCode.emplace_back(std::string("empty line"));
                node->irCode.emplace_back(std::string("dent out"));
                node->irCode.emplace_back(blockLabel.substr(1)+":");
                node->irCode.emplace_back(std::string("dent in"));
                auto elementVar = "%v"+std::to_string(++variableNum);
                node->irCode.emplace_back(elementVar+" = getelementptr "+typeToIrString(node->realType)+
                    ", ptr "+node->irResultPtr+", i32 0, i32 "+loadLoopVar);
                llvmStore(node->irCode, elementVar, child->irResult, child->realType);
                auto newLoopVar = "%v"+std::to_string(++variableNum);
                node->irCode.emplace_back(newLoopVar+" = add i32 "+loadLoopVar+", 1");
                llvmStore(node->irCode, loopVar, newLoopVar, I32);
                node->irCode.emplace_back("br label "+condLabel);

                node->irCode.emplace_back(std::string("empty line"));
                node->irCode.emplace_back(std::string("dent out"));
                node->irCode.emplace_back(endLabel.substr(1)+":");
                node->irCode.emplace_back(std::string("dent in"));
            }
        }
    }
    else if (node->type == astNodeType::STRUCT_BUILD)
    {
        node->irResult = node->irResultPtr = "%v"+std::to_string(++variableNum);
        node->irCode.emplace_back(node->irResultPtr+" = alloca "+node->children[0]->irResult);
        auto realT = typeToItem(node->children[0]->realType);
        for (int i=0; i<node->children[1]->children.size(); i++)
        {
            auto child = node->children[1]->children[i];
            auto id = realT.memberNames->at(child->value);
            auto tp = *realT.members[id];
            node->irCode.emplace_back(&child->children[0]->irCode);
            node->irCode.emplace_back("%v"+std::to_string(++variableNum)+" = getelementptr "+
                node->children[0]->irResult+", ptr "+node->irResultPtr+", i32 0, i32 "+std::to_string(id));
            llvmStore(node->irCode, "%v"+std::to_string(variableNum),
                child->children[0]->irResult, tp);
        }
    }
    else if (node->type == astNodeType::ARRAY_INDEX)
    {
        node->irCode.emplace_back(&node->children[0]->irCode);
        node->irCode.emplace_back(&node->children[1]->irCode);
        node->irResultPtr = "%v"+std::to_string(++variableNum);

        node->irCode.emplace_back(node->irResultPtr+" = getelementptr "+
            typeToIrString(node->children[0]->realType)+", ptr "+
            node->children[0]->irResult+", i32 0, i32 "+node->children[1]->irResult);
        if (isBigType(node->realType))
            node->irResult = node->irResultPtr;
        else
        {
            node->irResult = "%v"+std::to_string(++variableNum);
            llvmLoad(node->irCode, node->irResult, node->irResultPtr, node->realType);
        }
    }
    else if (node->type == astNodeType::SELF)
    {
        if (node->father->type == astNodeType::PARAMETERS)
            node->irResult = "%v"+std::to_string(++variableNum);
        else
        {
            node->irResultPtr = "%v"+std::to_string(node->variableID);
            if (isBigType(node->realType))
                node->irResult = node->irResultPtr;
            else
            {
                node->irResult = "%v"+std::to_string(++variableNum);
                llvmLoad(node->irCode, node->irResult, node->irResultPtr, node->realType);
            }
        }
    }

    // solve auto deref
    for (int t=0; t<node->autoDerefCount; t++)
    {
        node->realType = *node->realType.typePtr;
        node->irResultPtr = node->irResult;
        if (isBigType(node->realType))
            break;
        node->irResult = "%v"+std::to_string(++variableNum);
        llvmLoad(node->irCode, node->irResult, node->irResultPtr, node->realType);
    }

    // solve auto ref
    for (int t=0; t<-node->autoDerefCount; t++)
    {
        if (node->realType.name == TypeName::ARRAY || node->realType.name == TypeName::STRUCT)
            break;
        auto oldResult = node->irResult;
        node->irResult = "%v"+std::to_string(++variableNum);
        node->irCode.emplace_back(node->irResult+" = alloca ptr");
        llvmStore(node->irCode, node->irResult, oldResult, node->realType);
        node->realType = Type(TypeName::REF, new Type(node->realType), 0);
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

std::vector<std::any> loadBuiltinIr()
{
    std::vector<std::any> res;
    res.emplace_back(std::string("target triple = \"x86_64-unknown-linux-gnu\""));
    res.emplace_back(std::string("declare void @llvm.memcpy.p0.p0.i32(ptr, ptr, i32, i1)"));
    res.emplace_back(std::string("@.str = private unnamed_addr constant [3 x i8] c\"%d\\00\", align 1"));
    res.emplace_back(std::string("@.str.1 = private unnamed_addr constant [4 x i8] c\"%d\\0A\\00\", align 1"));
    res.emplace_back(std::string("define dso_local void @printInt(i32 noundef %x) local_unnamed_addr {"));
    res.emplace_back(std::string("entry:"));
    res.emplace_back(std::string("  %call = tail call noundef i32 (ptr, ...) @printf(ptr noundef nonnull @.str, i32 noundef %x)"));
    res.emplace_back(std::string("  ret void"));
    res.emplace_back(std::string("}"));
    res.emplace_back(std::string("declare noundef i32 @printf(ptr noundef, ...) local_unnamed_addr"));
    res.emplace_back(std::string("define dso_local void @printlnInt(i32 noundef %x) local_unnamed_addr {"));
    res.emplace_back(std::string("entry:"));
    res.emplace_back(std::string("  %call = tail call noundef i32 (ptr, ...) @printf(ptr noundef nonnull @.str.1, i32 noundef %x)"));
    res.emplace_back(std::string("  ret void"));
    res.emplace_back(std::string("}"));
    res.emplace_back(std::string("define dso_local i32 @getInt() local_unnamed_addr {"));
    res.emplace_back(std::string("entry:"));
    res.emplace_back(std::string("  %addr = alloca i32"));
    res.emplace_back(std::string("  %call = call noundef i32 (ptr, ...) @scanf(ptr noundef nonnull @.str, ptr noundef nonnull %addr)"));
    res.emplace_back(std::string("  %result = load i32, ptr %addr"));
    res.emplace_back(std::string("  ret i32 %result"));
    res.emplace_back(std::string("}"));
    res.emplace_back(std::string("declare noundef i32 @scanf(ptr noundef, ...) local_unnamed_addr"));
    res.emplace_back(std::string("declare void @exit(i32 noundef) local_unnamed_addr"));
    res.emplace_back(std::string("empty line"));
    return res;
}

std::vector<std::string> generateIr(astNode* node)
{
    updateIrState(node, node);
    int dent = 0;
    auto res = loadBuiltinIr();
    if (!node->irCodeStruct.empty())
        node->irCodeStruct.emplace_back(std::string("empty line"));
    for (const auto& ele:node->irCodeStruct)
        res.emplace_back(ele);
    for (const auto& ele:node->irCode)
        res.emplace_back(ele);
    return trimString(integrateCode(res, dent));
}
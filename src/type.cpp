#include "type.h"

#include <bits/chrono.h>

#include "token.h"

Type::Type():name(TypeName::ILLEGAL), typePtr(nullptr), len(-1){}

Type::Type(const TypeName& A):name(A), typePtr(nullptr), len(-1){}

Type::Type(const TypeName& A, const Type* B, const int& C):name(A), typePtr(B), len(C){}

bool operator==(const Type& A, const Type& B)
{
    if (A.name != B.name)
        return false;
    if (A.name == TypeName::ARRAY)
        return A.len == B.len && *A.typePtr == *B.typePtr;
    if(A.name == TypeName::STRUCT)
        return A.structID == B.structID;
    return true;
}
bool operator!=(const Type& A, const Type& B){return !(A==B);}
std::ostream& operator<<(std::ostream& os, const Type& T){return os<<T.to_string();}

std::string Type::to_string() const
{
    switch (name)
    {
        case TypeName::ILLEGAL: return "ILLEGAL";
        case TypeName::I32: return "i32";
        case TypeName::U32: return "u32";
        case TypeName::ISIZE: return "isize";
        case TypeName::USIZE: return "usize";
        case TypeName::UINT: return "uint";
        case TypeName::IINT: return "iint";
        case TypeName::INT: return "int";
        case TypeName::CHAR: return "char";
        case TypeName::STR: return "str";
        case TypeName::STRING: return "String";
        case TypeName::BOOL: return "bool";
        case TypeName::UNIT: return "unit";
        case TypeName::ENUM: return "(enum)" + structName;
        case TypeName::ARRAY: return "[" + typePtr->to_string()+";"+std::to_string(len)+"]";
        case TypeName::STRUCT: return "(struct)" + structName;
        case TypeName::TYPE: return typePtr->to_string()+"_T";
        default: return "who are you?";
    }
}

bool isNumber(const Type& T){return T == I32 || T == U32 || T == ISIZE || T == USIZE ||
    T == IINT || T == UINT || T == INT;}
bool isNumberB(const Type& T){return isNumber(T) || T == BOOL;}
bool isChar(const Type& T){return T == CHAR || T == STR || T == STRING;}

Type findScope(const std::pair<Scope*,astNode*> &scope, const std::string &name)
{
    auto res = scope.first->req(name);
    if (res != ILLEGAL)
        return res;
    if (scope.second == nullptr)
        return ILLEGAL;
    return findScope(scope.second->scope, name);
}

bool checkAsTrans(const Type &A, const Type &B)
{
    if (A == B)
        return true;
    if (A == BOOL)
        return B == I32 || B == U32 || B == ISIZE || B == USIZE;
    if (isNumberB(A) || A == CHAR)
        return B == I32 || B == U32 || B == ISIZE || B == USIZE;
    return false;
}

Type strToType(const std::string &name)
{
    if (name == "()")
        return UNIT;
    if (name == "i32")
        return I32;
    if (name == "u32")
        return U32;
    if (name == "isize")
        return ISIZE;
    if (name == "usize")
        return USIZE;
    if (name == "char")
        return CHAR;
    if (name == "bool")
        return BOOL;
    if (name == "str")
        return STR;
    if (name == "String")
        return STRING;
    return ILLEGAL;
}

void deriveNumberType(Type& A, Type &B)
{
    if (!isNumber(A) || !isNumber(B))
        throw compileError();
    if (A == I32 && (B == IINT || B == INT))
        B = A;
    else if (A == U32 && (B == UINT || B == INT))
        B = A;
    else if (A == ISIZE && (B == IINT || B == INT))
        B = A;
    else if (A == USIZE && (B == UINT || B == INT))
        B = A;
    else if (A == IINT && (B == I32 || B == ISIZE))
        A = B;
    else if (A == IINT && B == INT)
        B = A;
    else if (A == UINT && (B == U32 || B == USIZE))
        A = B;
    else if (A == UINT && B == INT)
        B = A;
    else if (A == INT)
        A = B;
    if (A != B)
        throw compileError();
}

void updateType(astNode* node)
{
    if (node->realType != ILLEGAL)
        return ;
    for (auto child:node->children)
        updateType(child);
    if (node->type == astNodeType::PROGRAM)
    {
        node->scope = std::make_pair(new Scope(), nullptr);

    }
    if (node->type == astNodeType::BINARY_OPERATOR)
    {
        auto T0 = node->children[0]->realType, T1  = node->children[1]->realType;
        auto E0 = node->children[0]->eval, E1  = node->children[1]->eval;
        if (node->value == "+" || node->value == "-" || node->value == "*" || node->value == "/" ||
            node->value == "%" || node->value == "<<" || node->value == ">>")
        {
            if (!isNumber(T0))
                throw compileError();
            deriveNumberType(T0, T1);
            node->realType = T0;
            if (E0.has_value() && E1.has_value())
            {
                auto L1 = std::any_cast<long long>(E0), L2 = std::any_cast<long long>(E1);
                // overflow? No one cares.
                if (node->value == "+")
                    node->eval = L1 + L2;
                else if (node->value == "-")
                    node->eval = L1 - L2;
                else if (node->value == "*")
                    node->eval = L1 * L2;
                else if (node->value == "/")
                    node->eval = L1 / L2;
                else if (node->value == "%")
                    node->eval = L1 % L2;
                else if (node->value == "<<")
                    node->eval = L1 << L2;
                else if (node->value == ">>")
                    node->eval = L1 >> L2;
            }
        }
        else if (node->value == "&" || node->value == "^" || node->value == "|")
        {
            if (T0 == BOOL && T1 == BOOL)
            {
                node->realType = T0;
                if (E0.has_value() && E1.has_value())
                {
                    auto L1 = std::any_cast<bool>(E0), L2 = std::any_cast<bool>(E1);
                    if (node->value == "&")
                        node->eval = L1 & L2;
                    else if (node->value == "^")
                        node->eval = L1 ^ L2;
                    else if (node->value == "|")
                        node->eval = L1 | L2;
                }
            }
            else
            {
                deriveNumberType(T0, T1);
                node->realType = T0;
                if (E0.has_value() && E1.has_value())
                {
                    auto L1 = std::any_cast<long long>(E0), L2 = std::any_cast<long long>(E1);
                    if (node->value == "&")
                        node->eval = L1 & L2;
                    else if (node->value == "^")
                        node->eval = L1 ^ L2;
                    else if (node->value == "|")
                        node->eval = L1 | L2;
                }
            }
        }
        else if (node->value == "<" || node->value == ">" || node->value == "<=" || node->value == ">=")
        {
            node->realType = BOOL;
            if (T0 == CHAR && T1 == CHAR)
            {
                if (E0.has_value() && E1.has_value())
                {
                    auto L0 = std::any_cast<char>(E0), L1 = std::any_cast<char>(E1);
                    if (node->value == "<")
                        node->eval = L0 < L1;
                    else if (node->value == ">")
                        node->eval = L0 > L1;
                    else if (node->value == "<=")
                        node->eval = L0 <= L1;
                    else if (node->value == ">=")
                        node->eval = L0 >= L1;
                }
            }
            else if (T0 == BOOL && T1 == BOOL)
            {
                if (E0.has_value() && E1.has_value())
                {
                    auto L1 = std::any_cast<bool>(E0), L2 = std::any_cast<bool>(E1);
                    if (node->value == "<")
                        node->eval = L1 < L2;
                    else if (node->value == ">")
                        node->eval = L1 > L2;
                    else if (node->value == "<=")
                        node->eval = L1 <= L2;
                    else if (node->value == ">=")
                        node->eval = L1 >= L2;
                }
            }
            else
            {
                deriveNumberType(T0, T1);
                if (E0.has_value() && E1.has_value())
                {
                    auto L1 = std::any_cast<long long>(E0), L2 = std::any_cast<long long>(E1);
                    if (node->value == "<")
                        node->eval = L1 < L2;
                    else if (node->value == ">")
                        node->eval = L1 > L2;
                    else if (node->value == "<=")
                        node->eval = L1 <= L2;
                    else if (node->value == ">=")
                        node->eval = L1 >= L2;
                }
            }
        }
        else if (node->value == "==" || node->value == "!=")
        {
            node->realType = BOOL;
            if (T0 == CHAR && T1 == CHAR)
            {
                if (E0.has_value() && E1.has_value())
                {
                    auto L1 = std::any_cast<char>(E0), L2 = std::any_cast<char>(E1);
                    if (node->value == "==")
                        node->eval = L1 == L2;
                    else if (node->value == "!=")
                        node->eval = L1 != L2;
                }
            }
            else if (T0 == BOOL && T1 == BOOL)
            {
                if (E0.has_value() && E1.has_value())
                {
                    auto L1 = std::any_cast<bool>(E0), L2 = std::any_cast<bool>(E1);
                    if (node->value == "==")
                        node->eval = L1 == L2;
                    else if (node->value == "!=")
                        node->eval = L1 != L2;
                }
            }
            else if (T0 == UNIT && T1 == UNIT)
            {
                if (E0.has_value() && E1.has_value())
                {
                    if (node->value == "==")
                        node->eval = true;
                    else if (node->value == "!=")
                        node->eval = false;
                }
            }
            else if (T0.name == TypeName::ENUM && T1.name == TypeName::ENUM)
            {

            }
            else
            {
                deriveNumberType(T0, T1);
                if (E0.has_value() && E1.has_value())
                {
                    auto L1 = std::any_cast<long long>(E0), L2 = std::any_cast<long long>(E1);
                    if (node->value == "==")
                        node->eval = L1 == L2;
                    else if (node->value == "!=")
                        node->eval = L1 != L2;
                }
            }
        }
        else if (node->value == "&&" || node->value == "||")
        {
            if (T0 != T1 || T0 != BOOL)
                throw compileError();
            node->realType = BOOL;
            if (E0.has_value() && E1.has_value())
            {
                auto L1 = std::any_cast<bool>(E0), L2 = std::any_cast<bool>(E1);
                if (node->value == "&&")
                    node->eval = L1 && L2;
                else if (node->value == "||")
                    node->eval = L1 || L2;
            }
        }
        else if (node->value == "=")
        {
            if (T0 != T1)
                throw compileError();
            node->realType = UNIT;
        }
        else if (node->value == "+=" || node->value == "-=" || node->value == "*=" || node->value == "/=" ||
            node->value == "%=" || node->value == "<<=" || node->value == ">>")
        {
            if (T0 != T1 || !isNumber(T0))
                throw compileError();
            node->realType = UNIT;
        }
        else if (node->value == "&=" || node->value == "^=" || node->value == "|=")
        {
            if (T0 != T1 || !isNumberB(T0))
                throw compileError();
            node->realType = UNIT;
        }
        else if (node->value == "as")
        {
            T1 = *T1.typePtr;
            checkAsTrans(T0, T1);
            std::cerr<<T1<<std::endl;
            node->realType = T1;
            if (E0.has_value())
            {
                if (T0 == T1)
                    node->eval = E0;
                else if (isNumber(T0))
                    node->eval = E0;
                else if (T0 == CHAR)
                    node->eval = static_cast<long long>(std::any_cast<char>(E0));
                else if (T0 == BOOL)
                    node->eval = static_cast<long long>(std::any_cast<bool>(E0));
            }
        }
    }
    else if (node->type == astNodeType::IDENTIFIER)
    {
    }
    else if (node->type == astNodeType::TYPE)
    {
        if (node->value == "i32")
            node->realType = I32_T;
        else if (node->value == "u32")
            node->realType = U32_T;
        else if (node->value == "isize")
            node->realType = ISIZE_T;
        else if (node->value == "usize")
            node->realType = USIZE_T;
        else if (node->value == "char")
            node->realType = CHAR_T;
        else if (node->value == "bool")
            node->realType = BOOL_T;
        else if (node->value == "str")
            node->realType = STR_T;
        else if (node->value == "String")
            node->realType = STRING_T;
        else if (node->value == "()")
            node->realType = UNIT_T;
    }
    else if (node->type == astNodeType::BOOL_LITERAL)
    {
        node->realType = BOOL;
        node->eval = node->value == "true" ? true : false;
    }
    else if (node->type == astNodeType::CHAR_LITERAL)
    {
        node->realType = CHAR;
        if (node->value[0] == '\\')
        {
            if (node->value[1] == 'n')
                node->eval = '\n';
            else if (node->value[1] == 't')
                node->eval = '\t';
            else if (node->value[1] == 'r')
                node->eval = '\r';
            else if (node->value[1] == '\\')
                node->eval = '\\';
            else if (node->value[1] == '\'')
                node->eval = '\'';
            else if (node->value[1] == '"')
                node->eval = '"';
            else if (node->value[1] == '0')
                node->eval = '\0';
            else
                throw compileError();
        }
        else
            node->eval = node->value[0];
    }
    else if (node->type == astNodeType::INTEGER_LITERAL)
    {
        std::string info;
        for (auto ch:node->value)
            if (ch != '_')
                info.push_back(ch);
        if (info.size() > 3 && info.substr(info.size() - 3, 3) == "i32")
        {
            node->realType = I32;
            node->eval = static_cast<long long>(std::stoi(info.substr(0, info.size()-3), nullptr, 0));
        }
        else if (info.size() > 3 && info.substr(info.size() - 3, 3) == "u32")
        {
            node->realType = U32;
            node->eval = static_cast<long long>(std::stoul(info.substr(0, info.size()-3), nullptr, 0));
        }
        else if (info.size() > 5 && info.substr(info.size() - 5, 5) == "isize")
        {
            node->realType = ISIZE;
            node->eval = static_cast<long long>(std::stoi(info.substr(0, info.size()-5), nullptr, 0));
        }
        else if (info.size() > 5 && info.substr(info.size() - 5, 5) == "usize")
        {
            node->realType = USIZE;
            node->eval = static_cast<long long>(std::stoul(info.substr(0, info.size()-5), nullptr, 0));
        }
        else // missing suffix, try to infer the type
        {
            auto res = std::stoll(info);
            node->eval = res;
            if (res > UINT_MAX || res < INT_MIN)
                throw compileError();
            if (res >= 0 && res <= INT_MAX)
                node->realType = INT;
            else if (res<0)
                node->realType = IINT;
            else
                node->realType = UINT;
        }
    }
    else if (node->type == astNodeType::STRING_LITERAL)
        node->realType = STRING;
    else if (node->type == astNodeType::EXPRESSION_STATEMENT)
    {
        if (node->children.empty())
        {
            node->realType = UNIT;
            node->eval = Unit();
        }
        else
        {
            node->realType = node->children[0]->realType;
            node->eval = node->children[0]->eval;
        }
    }
    else if (node->type == astNodeType::ARRAY_BUILD)
    {
        auto& children = node->children[0]->children;
        if (children.empty())
            throw compileError();
        auto& T = children[0]->realType;
        for (auto child:children)
            if (child->realType != T)
                throw compileError();
        node->realType = (Type){TypeName::ARRAY, &T, static_cast<int>(children.size())};
    }
    else if (node->type == astNodeType::ARRAY_INDEX)
    {
        auto T0 = node->children[0]->realType, T1 = node->children[1]->realType;
        if (T0.name != TypeName::ARRAY || T1 != USIZE)
            throw compileError();
        node->realType = *T0.typePtr;
    }
}

void semanticCheckType(astNode* node){updateType(node);}
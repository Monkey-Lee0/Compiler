#include "type.h"

#include<algorithm>

#include "token.h"

Type::Type():name(TypeName::ILLEGAL), typePtr(nullptr), len(-1){}

Type::Type(const TypeName& A):name(A), typePtr(nullptr), len(-1){}

Type::Type(const TypeName& A, Type* B, const unsigned int& C):name(A), typePtr(B), len(C){}

bool operator==(const Type& A, const Type& B)
{
    if (A.name != B.name)
        return false;
    if (A.name == TypeName::ARRAY)
        return A.len == B.len && *A.typePtr == *B.typePtr;
    if(A.name == TypeName::STRUCT)
        return A.structID == B.structID;
    if (A.name == TypeName::TYPE)
        return *A.typePtr == *B.typePtr;
    if (A.name == TypeName::FUNCTION)
    {
        bool check=*A.typePtr == *B.typePtr && A.members.size() == B.members.size();
        for (int i=0; i<A.members.size(); i++)
            check &= A.members[i] == B.members[i];
        return check;
    }
    if (A.name == TypeName::REF || A.name == TypeName::MUT_REF)
        return *A.typePtr == *B.typePtr;
    if (A.name == TypeName::ENUM)
    {
        return true;
    }
    return true;
}
bool operator!=(const Type& A, const Type& B){return !(A==B);}
std::ostream& operator<<(std::ostream& os, const Type& T){return os<<T.to_string();}

std::string Type::to_string() const
{
    std::string res="(";
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
        case TypeName::ENUM: return "(enum)" + structName;
        case TypeName::ARRAY: return "[" + typePtr->to_string()+";"+std::to_string(len)+"]";
        case TypeName::STRUCT:
            res="(struct" + std::to_string(structID)+")" + structName+"{";
            for (const auto& [var, info]:field->itemTable)
                if (info.isMutable)
                    res += var + ":" + info.type.to_string() + ",";
            if (res.back() == ',')
                res.pop_back();
            res += "}";
            return res;
        case TypeName::FUNCTION:
            for (int i=0;i<members.size();i++)
            {
                res.append(members[i]->to_string());
                if (i+1 != members.size())
                    res.append(",");
            }
            res.append(")->" + typePtr->to_string());
            return res;
        case TypeName::UNIT:return "unit";
        case TypeName::TYPE:
            return "("+typePtr->to_string()+")_T";
        case TypeName::VERSATILE: return "versatile";
        case TypeName::REF:return "&"+typePtr->to_string();
        case TypeName::MUT_REF:return "&mut "+typePtr->to_string();
        case TypeName::NEVER: return "never";
        default: return "who are you?";
    }
}

bool isNumber(const Type& T){return T == I32 || T == U32 || T == ISIZE || T == USIZE ||
    T == IINT || T == UINT || T == INT;}
bool isNumberB(const Type& T){return isNumber(T) || T == BOOL;}
bool isChar(const Type& T){return T == CHAR || T == STR || T == STRING;}

scopeInfo findScopeType(const std::pair<Scope*,astNode*> &scope, const std::string &name)
{;
    auto res = scope.first->getType(name);
    if (res.type != ILLEGAL || scope.second == nullptr)
        return res;
    return findScopeType(scope.second->scope, name);
}

scopeInfo findScopeItem(const std::pair<Scope*,astNode*> &scope, const std::string &name)
{;
    auto res = scope.first->getItem(name);
    if (res.type != ILLEGAL || scope.second == nullptr)
        return res;
    return findScopeItem(scope.second->scope, name);
}

void checkAsTrans(const Type &A, const Type &B)
{
    if (A == B || A == NEVER)
        return ;
    if ((isNumberB(A) || A == CHAR) && (B == I32 || B == U32 || B == ISIZE || B == USIZE))
        return ;
    throw compileError();
}

void deriveStrongTrans(const Type &A, const Type &B, bool canRemoveMut=true)
{
    if (A == B || A == NEVER || A == VERSATILE || B == VERSATILE)
        return ;
    if (A == INT && (B == IINT || B == UINT || B == I32 || B == U32 || B == ISIZE || B == USIZE))
        return ;
    if (A == IINT && (B == I32 || B == ISIZE))
        return ;
    if (A == UINT && (B == U32 || B == USIZE))
        return ;
    if (canRemoveMut && A.name == TypeName::MUT_REF && B.name == TypeName::REF)
        return deriveStrongTrans(*A.typePtr, *B.typePtr, false);
    if (A.name == TypeName::REF && B.name == TypeName::REF)
        return deriveStrongTrans(*A.typePtr, *B.typePtr, false);
    if (A.name == TypeName::MUT_REF && B.name == TypeName::MUT_REF)
        return deriveStrongTrans(*A.typePtr, *B.typePtr, false);
    if (A.name == TypeName::ARRAY && B.name == TypeName::ARRAY && A.len == B.len)
        return deriveStrongTrans(*A.typePtr, *B.typePtr, false);
    throw compileError();
}

Type itemToType(Type* A)
{
    if (*A == UNIT)
        return UNIT;
    return {TypeName::TYPE, A, 0};
}

Type itemToType(const Type& A)
{
    if (A == UNIT)
        return UNIT;
    return {TypeName::TYPE, new Type(A), 0};
}

Type& typeToItem(const Type& A)
{
    if (A == UNIT)
        return UNIT;
    if (A.name != TypeName::TYPE)
        throw compileError();
    return *A.typePtr;
}

Type castRef(Type& A){return {TypeName::REF, &A, 0};}

Type castMutRef(Type& A){return {TypeName::MUT_REF, &A, 0};}

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

void deriveAllType(Type& A, Type& B)
{
    if (A == VERSATILE)
        A = B;
    else if (B == VERSATILE)
        B = A;
    else if (A.name == TypeName::ARRAY || B.name == TypeName::ARRAY)
    {
        if (A.name != B.name || A.len != B.len)
            throw compileError();
        deriveAllType(*A.typePtr, *B.typePtr);
    }
    if (A != B)
        deriveNumberType(A, B);
}

bool isItem(const astNode* node)
{
    return node->type == astNodeType::CONST_STATEMENT || node->type == astNodeType::FUNCTION ||
        node->type == astNodeType::STRUCT || node->type == astNodeType::TRAIT ||
        node->type == astNodeType::IMPL;
}

void resolveDependency(astNode* node)
{
    auto getString=[](astNode* node)->std::string
    {
        if (node->type == astNodeType::STRUCT || node->type == astNodeType::IMPL)
            return "T-"+node->value;
        return node->value;
    };
    for (auto child:node->children)
        child->scope = node->scope;
    std::vector<std::string> name;
    for (const auto child:node->children)
        if (isItem(child))
            name.push_back(getString(child));
    std::unordered_map<std::string, astNode*> mp;
    std::ranges::sort(name);
    std::vector<std::vector<int>> G;
    std::vector<int> d;
    G.resize(name.size(),{});
    d.resize(name.size(), 0);
    for (const auto child:node->children)
        if (isItem(child))
            mp[getString(child)] = child;
    for (const auto child:node->children)
        if (isItem(child))
        {
            std::vector<astNode*> S;
            for (auto c:child->children)
                S.emplace_back(c);
            int X=std::ranges::lower_bound(name, getString(child))-name.begin();
            while (!S.empty())
            {
                auto x=S.back();S.pop_back();
                if (x->type == astNodeType::IDENTIFIER || x->type == astNodeType::TYPE)
                {
                    int Y=std::ranges::lower_bound(name,getString(x))-name.begin();
                    if (Y != name.size() && name[Y] == getString(x) && mp[name[Y]]->type != astNodeType::FUNCTION)
                        G[Y].push_back(X), d[X]++;
                }
                for (auto c:x->children)
                    S.emplace_back(c);
            }
        }
    std::vector<int> e;
    std::vector<int> seq;
    for (int i=0; i < G.size(); i++)
        if (!d[i])
            e.push_back(i);
    while (!e.empty())
    {
        auto x=e.back();e.pop_back();
        seq.push_back(x);
        for (auto t:G[x])
        {
            d[t]--;
            if (!d[t])
                e.push_back(t);
        }
    }
    if (seq.size() != G.size())
        throw compileError();
    auto children0 = node->children;
    node->children.clear();
    for (int i=0; i<G.size(); i++)
        node->children.push_back(mp[name[seq[i]]]);
    for (auto child:children0)
        if (!isItem(child))
            node->children.push_back(child);

    //resolve constant dependency
    for (auto child:node->children)
        if (child->type == astNodeType::CONST_STATEMENT)
        {
            child->children[0]->scope = child->scope;
            updateType(child->children[0], nullptr, nullptr, nullptr);
            child->children[1]->scope = child->scope;
            updateType(child->children[1], nullptr, nullptr, nullptr);
            auto T0 = typeToItem(child->children[0]->realType), T1 = child->children[1]->realType;
            deriveStrongTrans(T1, T0);
            if (!child->children[1]->eval.has_value())
                throw compileError();
            scopeInfo value = {T0, child->children[1]->eval, false, true};
            if (findScopeItem(child->scope, child->value).isGlobal) // shadow
                throw compileError();
            child->scope.first->setItem(child->value, value);
        }

    // resolve function dependency
        else if (child->type == astNodeType::FUNCTION)
        {
            child->children[0]->scope = child->scope;
            updateType(child->children[0], nullptr, nullptr, nullptr);
            child->children[1]->scope = child->scope;
            updateType(child->children[1], nullptr, nullptr, nullptr);\
            Type T(TypeName::FUNCTION, new Type(typeToItem(child->children[1]->realType)), -1);
            child->scope = std::make_pair(new Scope(), node);
            for (auto id:child->children[0]->children)
            {
                auto T0=typeToItem(id->children[0]->realType);
                child->scope.first->setItem(id->value, {T0, std::any(), false, false});
                T.members.push_back(new Type(T0));
            }
            if (findScopeItem(child->scope, child->value).isGlobal) // shadow
                throw compileError();
            node->scope.first->setItem(child->value, {T, std::any(), false, true});
        }

    // resolve struct dependency
        else if (child->type == astNodeType::STRUCT)
        {
            auto T=new Type(TypeName::STRUCT);
            T->structID = ++structNum;
            T->structName = child->value;
            T->field = new Scope();
            child->scope = std::make_pair(T->field, node);
            if (child->children.empty()){T->typePtr=&UNIT;}
            else
            {
                child->children[0]->scope = child->scope;
                updateType(child->children[0], nullptr, nullptr, nullptr);
                for (auto id:child->children[0]->children)
                {
                    auto T0=typeToItem(id->children[0]->realType);
                    child->scope.first->setItem("-"+id->value, {T0, std::any(), true, true});
                }
                T->memberFieldNum = T->field->itemTable.size();
            }
            if (findScopeType(child->scope, child->value).isGlobal) // shadow
                throw compileError();
            node->scope.first->setType(child->value, {itemToType(T)
                , std::any(), false, true});
        }
    //resolve impl dependency
        else if (child->type == astNodeType::IMPL && child->children.size() == 2)
        {
            auto T = findScopeType(child->scope, child->children[0]->value).type;
            if (T.name != TypeName::TYPE || T.typePtr->name != TypeName::STRUCT)
                throw compileError();
            T=*T.typePtr;
            child->scope = std::make_pair(T.field, node);
            child->children[0]->scope = child->children[1]->scope = child->scope;
            resolveDependency(child->children[1]);
        }
}

void updateType(astNode* node, astNode* father, astNode* loopPtr, astNode* fnPtr)
{
    if (node->realType != ILLEGAL)
        return ;
    if (node->type == astNodeType::STATEMENT_BLOCK)
        node->scope = std::make_pair(new Scope(), father);
    for (auto child:node->children)
        child->scope = node->scope;
    if (node->type == astNodeType::PROGRAM || node->type == astNodeType::STATEMENT_BLOCK)
        resolveDependency(node);

    if (node->type == astNodeType::WHILE || node->type == astNodeType::LOOP)
        loopPtr = node;
    if (node->type == astNodeType::FUNCTION)
        fnPtr = node;

    if (node->type == astNodeType::BINARY_OPERATOR && (node->value == "." || node->value == "::"))
        updateType(node->children[0], node, father, loopPtr);
    else
        for (int i=0 ;i<node->children.size(); i++)
        {
            auto child=node->children[i];
            node->curSonId=i;
            updateType(child, node, loopPtr, fnPtr);
            node->hasBreak |= child->hasBreak;
            node->hasReturn |= child->hasReturn;
        }

    if (node->type == astNodeType::FUNCTION)
        node->hasReturn = false;
    else if (node->type == astNodeType::WHILE)
        node->hasBreak = node->hasReturn = false;
    else if (node->type == astNodeType::LOOP)
        node->hasBreak = false;
    else if (node->type == astNodeType::IF && node->children.size() <= 2)
        node->hasReturn = node->hasBreak = false;
    else if (node->type == astNodeType::IF && node->children.size() == 3)
    {
        node->hasReturn = node->children[1]->hasReturn & node->children[2]->hasReturn;
        node->hasBreak = node->children[1]->hasBreak & node->children[2]->hasBreak;
    }
    else if (node->type == astNodeType::BREAK || node->type == astNodeType::CONTINUE)
        node->hasBreak = true;
    else if (node->type == astNodeType::RETURN)
        node->hasReturn = true;

    if (node->type == astNodeType::LET_STATEMENT)
    {
        auto T0 = typeToItem(node->children[1]->realType);
        if (node->children.size() == 3)
        {
            auto T1 = node->children[2]->realType;
            deriveStrongTrans(T1, T0);
        }
        scopeInfo value = {T0, std::any(),
            node->children[0]->value == "mut", false};
        if (findScopeItem(node->scope, node->value).isGlobal) // shadow a constant.
            throw compileError();
        node->scope.first->setItem(node->value, value);
    }
    else if (node->type == astNodeType::STATEMENT_BLOCK)
    {
        if (node->children.size() == 0)
            node->realType = UNIT;
        else
        {
            if (node->children.back()->type == astNodeType::RETURN_CUR)
                node->realType = node->children.back()->realType;
            else
            {
                node->realType = UNIT;
                for (auto child:node->children)
                    if (child->realType == NEVER || child->hasBreak || child->hasReturn)
                        node->realType = NEVER;
            }
            for (int i=0; i+1 < node->children.size(); i++)
                if (node->children[i]->type == astNodeType::RETURN_CUR &&
                    node->children[i]->realType != UNIT)
                    throw compileError();
        }
    }
    else if (node->type == astNodeType::FUNCTION)
    {
        auto T0 = typeToItem(node->children[1]->realType);
        auto T1 = node->children[2]->realType;
        deriveStrongTrans(T1, T0);
    }
    else if (node->type == astNodeType::RETURN_CUR)
    {
        node->realType = node->children[0]->realType;
        node->eval = node->children[0]->eval;
    }
    else if (node->type == astNodeType::RETURN)
    {
        auto T = UNIT;
        if (!node->children.empty())
            T = node->children.back()->realType;


        if (fnPtr == nullptr)
            throw compileError();
        auto T1=typeToItem(fnPtr->children[1]->realType);
        deriveStrongTrans(T, T1);
        node->realType = NEVER;
    }
    else if (node->type == astNodeType::IF)
    {
        if (node->children[0]->realType != BOOL)
            throw compileError();
        if (node->children.size() <= 2)
        {
            deriveStrongTrans(node->children[1]->realType, UNIT);
            node->realType = UNIT;
        }
        else
        {
            auto T0 = node->children[1]->realType;
            auto T1 = node->children[2]->realType;
            if (T0 == NEVER)
                T0 = T1;
            else if (T1 == NEVER)
                T1 = T0;
            deriveAllType(T0, T1);
            node->realType = T0;
        }
    }
    else if (node->type == astNodeType::ELSE)
    {
        if (father->type != astNodeType::IF)
            throw compileError();
        node->realType = node->children[0]->realType;
        node->eval = node->children[0]->eval;
    }
    else if (node->type == astNodeType::WHILE)
    {
        node->realType = UNIT;
    }
    else if (node->type == astNodeType::LOOP)
    {
        if (node->realType == ILLEGAL)
            node->realType = UNIT;
        deriveStrongTrans(node->children[0]->realType, UNIT);
    }
    else if (node->type == astNodeType::CONTINUE)
    {
        node->realType = NEVER;
        if (loopPtr == nullptr)
            throw compileError();
    }
    else if (node->type == astNodeType::BREAK)
    {
        node->realType = NEVER;
        if (loopPtr == nullptr)
            throw compileError();
        if (loopPtr->type == astNodeType::WHILE)
        {
            if (loopPtr->curSonId == 0 || !node->children.empty())
                throw compileError();
        }
        else
        {
            Type T = !node->children.empty()?node->children.back()->realType:UNIT;
            if (T != NEVER)
                if (loopPtr->realType == ILLEGAL)
                    loopPtr->realType = T;
                else
                    deriveAllType(loopPtr->realType, T);
        }
    }
    else if (node->type == astNodeType::BINARY_OPERATOR)
    {
        auto T0 = node->children[0]->realType, T1  = node->children[1]->realType;
        auto E0 = node->children[0]->eval, E1  = node->children[1]->eval;
        if (node->value == "+" || node->value == "-" || node->value == "*" || node->value == "/" ||
            node->value == "%" || node->value == "<<" || node->value == ">>")
        {
            if (T0.name == TypeName::REF)
                T0 = *T0.typePtr;
            if (T1.name == TypeName::REF)
                T1 = *T1.typePtr;
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
            if (T0.name == TypeName::REF)
                T0 = *T0.typePtr;
            if (T1.name == TypeName::REF)
                T1 = *T1.typePtr;
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
            while ((T0.name == TypeName::REF || T0.name == TypeName::MUT_REF)&&
                (T1.name == TypeName::REF || T1.name == TypeName::MUT_REF))
                T0 = *T0.typePtr, T1 = *T1.typePtr;
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
            while ((T0.name == TypeName::REF || T0.name == TypeName::MUT_REF)&&
                (T1.name == TypeName::REF || T1.name == TypeName::MUT_REF))
                T0 = *T0.typePtr, T1 = *T1.typePtr;
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
            else if ((T0 == NEVER || T0 == UNIT) && (T1 == NEVER || T1 == UNIT))
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
            if (T0.name == TypeName::REF)
                T0 = *T0.typePtr;
            if (T1.name == TypeName::REF)
                T1 = *T1.typePtr;
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
            if (!node->children[0]->isMutable)
                throw compileError();
            if (T1.name == TypeName::MUT_REF && T0.name == TypeName::REF)
                T1.name = T0.name;
            deriveStrongTrans(T1, T0);
            node->realType = UNIT;
        }
        else if (node->value == "+=" || node->value == "-=" || node->value == "*=" || node->value == "/=" ||
            node->value == "%=" || node->value == "<<=" || node->value == ">>")
        {
            if (T1.name == TypeName::REF)
                T1 = *T1.typePtr;
            if (!node->children[0]->isMutable)
                throw compileError();
            deriveNumberType(T0, T1);
            node->realType = UNIT;
        }
        else if (node->value == "&=" || node->value == "^=" || node->value == "|=")
        {
            if (T1.name == TypeName::REF)
                T1 = *T1.typePtr;
            if (!node->children[0]->isMutable)
                throw compileError();
            deriveAllType(T0, T1);
            if (!isNumberB(T0))
                throw compileError();
            node->realType = UNIT;
        }
        else if (node->value == "as")
        {
            T1 = typeToItem(T1);
            checkAsTrans(T0, T1);
            node->realType = T1;
            if (E0.has_value())
            {
                if (T0 == T1 || isNumber(T0))
                    node->eval = E0;
                else if (T0 == CHAR)
                    node->eval = static_cast<long long>(std::any_cast<char>(E0));
                else if (T0 == BOOL)
                    node->eval = static_cast<long long>(std::any_cast<bool>(E0));
            }
        }
        else if (node->value == ".")
        {
            auto T=node->children[0]->realType;
            node->isMutable = node->children[0]->isMutable;
            node->isVariable = node->children[0]->isVariable;
            while (T.name == TypeName::REF || T.name == TypeName::MUT_REF)
            {
                if (T.name == TypeName::REF)
                    node->isMutable = false;
                else
                    node->isMutable = true;
                T = *T.typePtr;
            }
            if (T.name != TypeName::STRUCT || node->children[1]->type != astNodeType::IDENTIFIER)
                throw compileError();
            auto T0 = T.field->getItem("-"+node->children[1]->value);
            if (T0.type == ILLEGAL)
                throw compileError();
            node->realType = T0.type;
            node->isMutable &= T0.isMutable;
        }
        else if (node->value == "::")
        {
            auto T=node->children[0]->realType;
            while (T.name == TypeName::REF || T.name == TypeName::MUT_REF)
                T = *T.typePtr;
            if (T.name != TypeName::TYPE || T.typePtr->name != TypeName::STRUCT)
                throw compileError();
            T = *T.typePtr;
            auto T0 = T.field->getItem(node->children[1]->value);
            if (T0.type == ILLEGAL)
                throw compileError();
            node->realType = T0.type;
            node->isVariable = node->children[1]->isVariable;
            node->eval = T0.eval;
        }
    }
    else if (node->type == astNodeType::UNARY_OPERATOR)
    {
        auto &T=node->children[0]->realType;
        auto E=node->children[0]->eval;
        if (node->value == "-")
        {
            auto T0 = IINT;
            deriveNumberType(T, T0);
            node->realType = T;
            if (E.has_value())
                node->eval = -std::any_cast<long long>(E);
        }
        else if (node->value == "!")
        {
            if (!isNumberB(T))
                throw compileError();
            node->realType = T;
            if (E.has_value())
            {
                if (E.type() == typeid(long long))
                    node->eval = ~std::any_cast<long long>(E);
                else if (E.type() == typeid(bool))
                    node->eval = !std::any_cast<bool>(E);
            }
        }
        else if (node->value == "&")
        {
            if (T.name == TypeName::TYPE)
                node->realType = itemToType(castRef(typeToItem(T)));
            else
                node->realType = castRef(T);
        }
        else if (node->value == "&mut")
        {
            if (T.name == TypeName::TYPE)
                node->realType = itemToType(castMutRef(typeToItem(T)));
            else
                node->realType = castMutRef(T);
            if (node->children[0]->isVariable && !node->children[0]->isMutable)
                throw compileError();
        }
        else if (node->value == "*")
        {
            if (T.name != TypeName::REF && T.name != TypeName::MUT_REF)
                throw compileError();
            if (T.name == TypeName::MUT_REF)
                node->isMutable = true;
            else
                node->isMutable = false;
            node->isVariable = true;
            node->realType = *T.typePtr;
        }
    }
    else if (node->type == astNodeType::IDENTIFIER)
    {
        scopeInfo R;
        if (father->type == astNodeType::BINARY_OPERATOR && father->value == "as" && father->curSonId == 1)
            R=findScopeType(node->scope, node->value);
        else if (father->type == astNodeType::BINARY_OPERATOR && father->value == "::" && father->curSonId == 0)
            R=findScopeType(node->scope, node->value);
        else if (father->type == astNodeType::STRUCT_BUILD && father->curSonId == 0)
            R=findScopeType(node->scope, node->value);
        else
        {
            R=findScopeItem(node->scope, node->value);
            if (R.type == ILLEGAL)
            {
                R=findScopeType(node->scope, node->value);
                auto P=typeToItem(R.type);
                if (P.name == TypeName::STRUCT && P.typePtr != nullptr)
                    R.type=P;
            }
        }
        if (R.type == ILLEGAL)
            throw compileError();
        node->isMutable = R.isMutable;
        node->isVariable = true;
        node->eval = R.eval;
        node->realType = R.type;
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
            node->realType = UNIT;
        else if (node->value == "_")
            node->realType = VERSATILE_T;
        else if (node->children.empty())
        {
            const auto T = findScopeType(node->scope, node->value).type;
            if (T.name != TypeName::TYPE)
                throw compileError();
            node->realType = T;
        }
        else if (node->children.size() == 2)
        {
            auto E = node->children[1]->eval;
            auto T = node->children[1]->realType;
            if (!E.has_value() || (T != USIZE && T != UINT && T!=INT))
                throw compileError();
            node->realType.name = TypeName::TYPE;
            node->realType.typePtr = new Type(TypeName::ARRAY, node->children[0]->realType.typePtr,
                static_cast<unsigned int>(std::any_cast<long long>(E)));
        }
        else
            throw compileError();
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
            node->isMutable = node->children[0]->isMutable;
            node->isVariable = node->children[0]->isVariable;
        }
    }
    else if (node->type == astNodeType::ARRAY_BUILD)
    {
        if (node->children.size() == 1) // [a,b,c]
        {
            auto& children = node->children[0]->children;
            if (children.empty())
                throw compileError();
            auto& T = children[0]->realType;
            std::vector<std::any> R;
            bool flag=true;
            for (auto child:children)
            {
                if (T == NEVER)
                    T = child->realType;
                else if (child->realType == NEVER)
                    child->realType = NEVER;
                else
                    deriveAllType(child->realType, T);
                if (!child->eval.has_value())
                    flag=false;
                if (flag)
                    R.emplace_back(child->eval);
            }
            node->realType = (Type){TypeName::ARRAY, &T, static_cast<unsigned int>(children.size())};
            if (flag)
                node->eval = R;
        }
        else // [a;b]
        {
            auto T = node->children[1]->realType;
            auto E = node->children[1]->eval;
            if ((T != USIZE && T != UINT && T != INT) || !E.has_value())
                throw compileError();
            auto len=static_cast<unsigned int>(std::any_cast<long long>(E));
            node->realType = (Type){TypeName::ARRAY, &node->children[0]->realType, len};
            auto E0 = node->children[0]->eval;
            if (E0.has_value())
            {
                std::vector<std::any> R;
                for (int i=0;i<len;i++)
                    R.push_back(E0);
                node->eval = R;
            }
        }
    }
    else if (node->type == astNodeType::ARRAY_INDEX)
    {
        auto T0 = node->children[0]->realType, T1 = node->children[1]->realType;
        auto E0 = node->children[0]->eval, E1 = node->children[1]->eval;
        node->isMutable = node->children[0]->isMutable;
        node->isVariable = node->children[0]->isVariable;
        while (T0.name == TypeName::REF || T0.name == TypeName::MUT_REF)
        {
            if (T0.name == TypeName::REF)
                node->isMutable = false;
            else
                node->isMutable = true;
            T0 = *T0.typePtr;
        }
        if (T0.name != TypeName::ARRAY || (T1 != USIZE && T1 != UINT && T1 != INT))
            throw compileError();
        node->realType = *T0.typePtr;
        if (E1.has_value())
        {
            auto L = static_cast<unsigned int>(std::any_cast<long long>(E1));
            if (T0.len <= L)
                throw compileError();
            if (E0.has_value())
            {
                auto R = std::any_cast<std::vector<std::any>>(E0);
                node->eval = R[L];
            }
        }
    }
    else if (node->type == astNodeType::FUNCTION_CALL)
    {
        auto FT=node->children[0]->realType;
        if (FT.name != TypeName::FUNCTION || FT.members.size() != node->children[1]->children.size())
            throw compileError();
        for (int i=0; i<FT.members.size(); i++)
        {
            auto T0 = *FT.members[i], T1 = node->children[1]->children[i]->realType;
            deriveStrongTrans(T1, T0);
        }
        node->realType = *FT.typePtr;
    }
    else if (node->type == astNodeType::STRUCT_BUILD)
    {
        auto T=node->children[0]->realType;
        if (T.name != TypeName::TYPE || T.typePtr->name != TypeName::STRUCT)
            throw compileError();
        auto T0 = T.typePtr;
        std::vector<std::string> Name;
        for (auto child:node->children[1]->children)
        {
            auto name=child->value;
            auto T1=T0->field->getItem("-"+name).type, T2=child->children[0]->realType;
            deriveStrongTrans(T2, T1);
            Name.push_back(name);
        }
        std::ranges::sort(Name);
        Name.erase(std::ranges::unique(Name).begin(), Name.end());
        if (Name.size() != T0->memberFieldNum ||
            Name.size() != node->children[1]->children.size())
            throw compileError();
        node->realType = *T0;
    }
}

void loadBuiltin(astNode* node)
{
    node->scope = std::make_pair(new Scope(), nullptr);
    auto T = (Type){TypeName::FUNCTION, &UNIT, 0};
    T.members.push_back(&I32);

    node->scope.first->setItem("exit", {T, std::any(), false, true});
    node->scope.first->setItem("printInt", {T, std::any(), false, true});
    node->scope.first->setItem("printlnInt", {T, std::any(), false, true});

    T.typePtr = &I32;
    T.members.clear();
    node->scope.first->setItem("getInt", {T, std::any(), false, true});
}

void semanticCheckType(astNode* node)
{
    loadBuiltin(node);
    updateType(node, nullptr, nullptr, nullptr);
}
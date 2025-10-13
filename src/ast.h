#ifndef AST_H
#define AST_H

#include<vector>
#include<string>
#include<unordered_map>
#include<any>

enum class TypeName
{
    ILLEGAL, I32, U32, ISIZE, USIZE, INT, IINT, UINT, CHAR, STR,
    UNIT, STRING, BOOL, ARRAY, ENUM, STRUCT, FUNCTION, METHOD, TYPE, REF, MUT_REF,
    VERSATILE, NEVER
};


class Scope;

inline unsigned int structNum = 0;
class Type final
{
public:
    TypeName name;
    Type* typePtr;
    Type* SelfPtr;
    bool isExit = false;
    bool selfMutable = false;
    unsigned int len, structID = 0;
    std::string structName;
    std::vector<Type const*> members;
    std::unordered_map<std::string, unsigned int>* memberNames;
    Scope* field = nullptr;
    unsigned int memberFieldNum = 0;
    [[nodiscard]] std::string to_string() const;
    Type();
    explicit Type(const TypeName&);
    Type(const TypeName&, Type*, const unsigned int&);
    friend bool operator==(const Type&, const Type&);
    friend bool operator!=(const Type&, const Type&);
    friend std::ostream& operator<<(std::ostream&, const Type&);
};

inline unsigned long long variableNum = 0;

class scopeInfo final
{
public:
    scopeInfo()=default;
    scopeInfo(const Type& a, const std::any& b, const bool& c, const bool & d,
        const unsigned long long& e):type(a), eval(b), isMutable(c), isGlobal(d), ID(e){}
    Type type;
    std::any eval;
    bool isMutable;
    bool isGlobal;
    unsigned long long ID;
};

class Scope final
{
public:
    std::unordered_map<std::string, scopeInfo> typeTable, itemTable;
    scopeInfo& getType(const std::string&);
    scopeInfo& getItem(const std::string&);
    void setType(const std::string&, const scopeInfo&);
    void setItem(const std::string&, const scopeInfo&);
};

enum class astNodeType
{
    PROGRAM, STRUCT, FIELDS, FIELD, FUNCTION, PARAMETERS, QUANTIFIER, ENUM, IMPL, TRAIT,
    ASSOCIATED_ITEM, STATEMENT_BLOCK, LET_STATEMENT, CONST_STATEMENT, EXPRESSION_STATEMENT, GROUP_EXPRESSION,
    TYPE, IDENTIFIER, TYPED_IDENTIFIER, BREAK, CONTINUE, RETURN, RETURN_CUR, LOOP, WHILE, IF, ELSE, UNARY_OPERATOR,
    BINARY_OPERATOR, FUNCTION_CALL, ARRAY_INDEX, STRUCT_BUILD, ARRAY_BUILD, CHAR_LITERAL, STRING_LITERAL,
    INTEGER_LITERAL, FLOAT_LITERAL, RAW_STRING_LITERAL, BOOL_LITERAL, SELF
};

class astNode final
{
public:
    ~astNode()=default;
    // used in parser
    std::vector<astNode*> children;
    std::string value;
    astNodeType type;
    // used in semantic check
    astNode* father;
    Type realType;
    std::any eval;
    std::pair<Scope*, astNode*> scope;
    unsigned int curSonId=0;
    bool hasBreak=false, hasReturn=false;
    bool hasAbsoluteBreak=false;
    bool isMutable=false;
    bool isVariable=false;
    // used in IR
    std::string irResult;
    std::string irResultPtr;
    unsigned long long variableID = 0;
    std::vector<std::any> irCode;

    std::vector<std::string> showSelf() const;
    std::vector<std::string> showTree() const;
};


#endif

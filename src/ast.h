#ifndef AST_H
#define AST_H

#include<vector>
#include<string>
#include<unordered_map>
#include<any>

enum class TypeName
{
    ILLEGAL, I32, U32, ISIZE, USIZE, INT, IINT, UINT, CHAR, STR,
    UNIT, STRING, BOOL, ARRAY, ENUM, STRUCT, FUNCTION, TYPE
};

class Type final
{
public:
    TypeName name;
    const Type* typePtr;
    int len, structID = 0;
    std::string structName;
    std::vector<Type const*> members;
    [[nodiscard]] std::string to_string() const;
    Type();
    explicit Type(const TypeName&);
    Type(const TypeName&, const Type*, const int&);
    friend bool operator==(const Type&, const Type&);
    friend bool operator!=(const Type&, const Type&);
    friend std::ostream& operator<<(std::ostream&, const Type&);
};

class scopeInfo final
{
public:
    Type type;
    std::any eval;
    bool isMutable;
};

class Scope final
{
    std::unordered_map<std::string, scopeInfo> table;
public:
    scopeInfo get(const std::string&);
    void set(const std::string&, const scopeInfo&);
};

enum class astNodeType
{
    PROGRAM, STRUCT, FIELDS, FIELD, FUNCTION, PARAMETERS, QUANTIFIER, ENUM, ENUM_MEMBERS, IMPL, TRAIT,
    ASSOCIATED_ITEM, STATEMENT_BLOCK, LET_STATEMENT, CONST_STATEMENT, EXPRESSION_STATEMENT, GROUP_EXPRESSION,
    TYPE, IDENTIFIER, TYPED_IDENTIFIER, BREAK, CONTINUE, RETURN, RETURN_CUR, LOOP, WHILE, IF, ELSE, UNARY_OPERATOR,
    BINARY_OPERATOR, FUNCTION_CALL, ARRAY_INDEX, STRUCT_BUILD, ARRAY_BUILD, CHAR_LITERAL, STRING_LITERAL,
    INTEGER_LITERAL, FLOAT_LITERAL, RAW_STRING_LITERAL, BOOL_LITERAL, SELF
};

class astNode final
{
public:
    virtual ~astNode()=default;
    std::vector<astNode*> children;
    std::string value;
    astNodeType type;
    Type realType;
    std::any eval;
    std::pair<Scope*, astNode*> scope;
    std::vector<std::string> showSelf();
    std::vector<std::string> showTree();
};


#endif

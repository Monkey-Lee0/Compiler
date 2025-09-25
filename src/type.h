#ifndef COMPILER_TYPE_H
#define COMPILER_TYPE_H

#include "ast.h"
#include<climits>

inline auto ILLEGAL = Type(TypeName::ILLEGAL);
inline auto I32 = Type(TypeName::I32);
inline auto U32 = Type(TypeName::U32);
inline auto ISIZE = Type(TypeName::ISIZE);
inline auto USIZE = Type(TypeName::USIZE);
inline auto INT = Type(TypeName::INT);
inline auto IINT = Type(TypeName::IINT);
inline auto UINT = Type(TypeName::UINT);
inline auto CHAR = Type(TypeName::CHAR);
inline auto STR = Type(TypeName::STR);
inline auto STRING = Type(TypeName::STRING);
inline auto BOOL = Type(TypeName::BOOL);
inline auto UNIT = Type(TypeName::UNIT);
inline auto VERSATILE = Type(TypeName::VERSATILE);
inline auto NEVER = Type(TypeName::NEVER);

inline auto I32_T = Type(TypeName::TYPE, &I32, -1);
inline auto U32_T = Type(TypeName::TYPE, &U32, -1);
inline auto ISIZE_T = Type(TypeName::TYPE, &ISIZE, -1);
inline auto USIZE_T = Type(TypeName::TYPE, &USIZE, -1);
inline auto CHAR_T = Type(TypeName::TYPE, &CHAR, -1);
inline auto STR_T = Type(TypeName::TYPE, &STR, -1);
inline auto STRING_T = Type(TypeName::TYPE, &STRING, -1);
inline auto BOOL_T = Type(TypeName::TYPE, &BOOL, -1);
inline auto VERSATILE_T = Type(TypeName::TYPE, &VERSATILE, -1);

struct Unit{};

scopeInfo findScope(const std::pair<Scope*, astNode*>&, const std::string &name);

void updateType(astNode*, astNode*, astNode*, astNode*);

bool checkAsTrans(const Type&, const Type&);

void loadBuiltin(astNode*);

void semanticCheckType(astNode*);


#endif
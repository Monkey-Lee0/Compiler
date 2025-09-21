#ifndef COMPILER_TYPE_H
#define COMPILER_TYPE_H

#include<iostream>
#include "ast.h"

const auto ILLEGAL = Type(TypeName::ILLEGAL);
const auto I32 = Type(TypeName::I32);
const auto U32 = Type(TypeName::U32);
const auto ISIZE = Type(TypeName::ISIZE);
const auto USIZE = Type(TypeName::USIZE);
const auto INT = Type(TypeName::INT);
const auto IINT = Type(TypeName::IINT);
const auto UINT = Type(TypeName::UINT);
const auto CHAR = Type(TypeName::CHAR);
const auto STR = Type(TypeName::STR);
const auto STRING = Type(TypeName::STRING);
const auto BOOL = Type(TypeName::BOOL);
const auto UNIT = Type(TypeName::UNIT);

const auto I32_T = Type(TypeName::TYPE, &I32, -1);
const auto U32_T = Type(TypeName::TYPE, &U32, -1);
const auto ISIZE_T = Type(TypeName::TYPE, &ISIZE, -1);
const auto USIZE_T = Type(TypeName::TYPE, &USIZE, -1);
const auto CHAR_T = Type(TypeName::TYPE, &CHAR, -1);
const auto STR_T = Type(TypeName::TYPE, &STR, -1);
const auto STRING_T = Type(TypeName::TYPE, &STRING, -1);
const auto BOOL_T = Type(TypeName::TYPE, &BOOL, -1);
const auto UNIT_T = Type(TypeName::TYPE, &UNIT, -1);

struct Unit{};

Type findScope(const std::pair<Scope*, astNode*>&, const std::string &name);

void updateType(astNode*);

bool checkAsTrans(const Type&, const Type&);

void semanticCheckType(astNode*);

#endif
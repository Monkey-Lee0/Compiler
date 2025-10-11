#ifndef IR_H
#define IR_H

#include"ast.h"
#include"semantic.h"

void updateIrState(astNode* node);

void loadBuiltinIr(astNode*);

std::vector<std::string> generateIr(astNode*);

#endif
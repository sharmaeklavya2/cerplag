#ifndef H_CODEGEN
#define H_CODEGEN

#include <stdbool.h>
#include "ast.h"
#include "ircode.h"

extern bool allow_codegen;

void codegen_chain(pAstNode p, IRCode* irc);
void destroy_code_chain(pAstNode p);
void codegen(pAstNode p);
void optimize_ircode(IRCode* code);

#endif  // H_CODEGEN

#ifndef H_COMPILER
#define H_COMPILER

#include <stdbool.h>
#include "symbol_table.h"
#include "ast.h"

void codegen(pAstNode p);
void compile_node(pAstNode p, pSD psd, const char* func_name);

void compile_program(ProgramNode* root, pSD psd, bool code_gen, bool destroy_module_bodies);
int compiler_main(FILE* ifp, FILE* ofp, FILE* sdfp, int level, int verbosity);

#endif  // H_COMPILER

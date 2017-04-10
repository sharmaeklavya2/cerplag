#ifndef H_AST
#define H_AST

#include <stdio.h>
#include "ast_defs.h"

void complain_ast_node_type(const char* funcname, astn_t node_type);

pAstNode get_ast_node(astn_t node_type);
pAstNode get_next_ast_node(pAstNode p);
void print_ast(FILE* fp, pAstNode p, int indent);
void destroy_ast(pAstNode p);

#endif  // H_AST

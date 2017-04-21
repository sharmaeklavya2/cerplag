#ifndef H_AST_GEN
#define H_AST_GEN

#include <stdio.h>
#include "parse_tree.h"
#include "ast.h"

void build_ast(parse_tree_node*);

int ast_gen_main(FILE* ifp, FILE* ofp, FILE* statsfp, int verbosity);

#endif  // H_AST_GEN

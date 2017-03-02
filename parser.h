#ifndef H_PARSER
#define H_PARSER

#include "parser_defs.h"
#include "util/tree.h"

extern char* GS_STRS[];

void init_parser();
void destroy_parser();

int parser_main(FILE* in, FILE* out, int verbosity, tree_printer tp);

tree_printer print_tree, print_tree_sub, dont_print_tree;

TreeNode* build_parse_tree(FILE* ifp, gsymb_t start_sym);

#endif  // H_PARSER

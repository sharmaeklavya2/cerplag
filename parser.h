#ifndef H_PARSER
#define H_PARSER

#include "parser_defs.h"
#include "util/tree.h"

extern char* GS_STRS[];

int parser_main(FILE* in, FILE* out, int verbosity);

TreeNode* build_parse_tree(FILE* ifp, gsymb_t start_sym);

#endif  // H_PARSER

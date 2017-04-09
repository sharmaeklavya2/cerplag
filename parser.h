#ifndef H_PARSER
#define H_PARSER

#include "parse_tree.h"

extern int parser_error_count;
extern pch_int_hmap intern_table;

gsymb_t init_parser();
void destroy_parser(bool destroy_intern_table);

int parser_main(FILE* in, FILE* out, int verbosity, parse_tree_printer tp);

parse_tree_printer print_tree, print_tree_sub, dont_print_tree;

parse_tree_node* build_parse_tree(FILE* ifp, gsymb_t start_sym);

#endif  // H_PARSER

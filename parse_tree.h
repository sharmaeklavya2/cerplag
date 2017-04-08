#ifndef H_PARSE_TREE
#define H_PARSE_TREE

#include "parser_defs.h"
typedef Symbol* parse;

#define TYPE parse
#define TYPED(x) parse_##x

#include "util/tree.gen.h"

#undef TYPE
#undef TYPED

#endif  // H_PCH_INT_HMAP

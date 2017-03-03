/* Batch no. 13
2014A7PS130P: Eklavya Sharma
2014A7PS023P: Daivik Nema */
#ifndef H_PARSE_TREE
#define H_PARSE_TREE

#include "parserDefs.h"
typedef Symbol* parse;

#define TYPE parse
#define TYPED(x) parse_##x

#include "tree.gen.h"

#undef TYPE
#undef TYPED

#endif  // H_PCH_INT_HMAP

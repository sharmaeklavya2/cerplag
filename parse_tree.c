#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "parse_tree.h"

void parse_destroy(parse x) {free(x);}
void parse_print(parse x, FILE* fp)
{psymbol_print(x, fp);}

#define TYPE parse
#define TYPED(x) parse_##x

#include "tree.gen.c"

#undef TYPE
#undef TYPED

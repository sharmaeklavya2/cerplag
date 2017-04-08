#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "parse_tree.h"

void parse_destroy(parse x) {free(x);}

#define TYPE parse
#define TYPED(x) parse_##x

#include "util/tree.gen.c"

#undef TYPE
#undef TYPED

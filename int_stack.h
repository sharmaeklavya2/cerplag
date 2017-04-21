#ifndef H_INT_STACK
#define H_INT_STACK

#define TYPE int
#define TYPED(x) int_##x

#include "stack.gen.h"

#undef TYPE
#undef TYPED

#endif  // H_INT_STACK

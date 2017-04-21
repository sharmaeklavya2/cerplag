#ifndef H_OP
#define H_OP

typedef enum {
#define X(a, b) OP_##a,
#include "data/op.xmac"
#undef X
    OP_LAST
} op_t;

#define NUM_OP OP_LAST

extern char* OP_STRS[];

#endif  // H_OP

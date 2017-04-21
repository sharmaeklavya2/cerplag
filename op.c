#include "op.h"

char* OP_STRS[] = {
#define X(a, b) b,
#include "data/op.xmac"
#undef X
};

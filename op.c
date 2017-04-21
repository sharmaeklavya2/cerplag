#include "op.h"

char* OP_STRS[] = {
#define X(a, b) b,
#include "op.xmac"
#undef X
};

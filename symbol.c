#include "symbol_defs.h"

char* GS_STRS[] = {
#define X(a, b) #a,
#include "data/tok.xmac"
#undef X
#define X(a) #a,
#include "data/nonterms.xmac"
#undef X
};

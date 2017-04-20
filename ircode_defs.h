#ifndef H_IRCODE_DEFS
#define H_IRCODE_DEFS

#include "op.h"

struct AddrNode;

typedef struct IRInstr {
    // Intermediate Representation Instruction
    op_t op;
    struct AddrNode *arg1, *arg2, *res;
    struct IRInstr *prev, *next;
    int label;
} IRInstr;

typedef IRInstr* pIRInstr;

#define TYPE pIRInstr
#define TYPED(x) irinstr_##x
#define CLASS IRCode
#define CLASSED(x) ircode_##x
#include "util/llist.gen.h"
#undef TYPE
#undef TYPED
#undef CLASS
#undef CLASSED

#include "addr.h"

#endif  // H_IRCODE_DEFS

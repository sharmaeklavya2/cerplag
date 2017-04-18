#ifndef H_IRCODE_DEFS
#define H_IRCODE_DEFS

#include "op.h"

struct AddrNode;

typedef struct IRInstr {
    // Intermediate Representation Instruction
    op_t op;
    struct AddrNode *arg1, *arg2, *res;
    struct IRInstr *prev, *next;
} IRInstr;

typedef struct IRCode {
    struct IRInstr *first, *last;
    int size;
} IRCode;

#include "addr.h"

#endif  // H_IRCODE_DEFS

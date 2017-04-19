#ifndef H_X86_DEFS
#define H_X86_DEFS

typedef enum {
#define X(a) X86_OP_##a,
#include "data/x86_opcode.xmac"
#undef X
    X86_OP_LAST
} x86_op_t;

#define NUM_X86_OPS X86_OP_LAST

extern const char* X86_OP_STRS[];

#define X86_INDENT_STR "    "

#define X86_ARG_SIZE 20

typedef struct X86Instr {
    x86_op_t opcode;
    char arg1[X86_ARG_SIZE];
    char arg2[X86_ARG_SIZE];
    struct X86Instr *prev, *next;
} X86Instr;

typedef X86Instr* pX86Instr;

#define TYPE pX86Instr
#define TYPED(x) x86_instr_##x
#define CLASS X86Code
#define CLASSED(x) x86_code_##x
#include "util/llist.gen.h"
#undef TYPE
#undef TYPED
#undef CLASS
#undef CLASSED

#endif  // H_X86_DEFS

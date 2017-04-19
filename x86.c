#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "x86.h"

const char* X86_OP_STRS[NUM_X86_OPS] = {
#define X(a) #a,
#include "data/x86_opcode.xmac"
#undef X
};

void x86_instr_init(X86Instr* instr, x86_op_t opcode) {
    memset((void*)instr, 0, sizeof(instr));
    instr->next = instr->prev = NULL;
    instr->arg1[0] = instr->arg2[0] = '\0';
    instr->opcode = opcode;
}

X86Instr* x86_instr_new(x86_op_t opcode) {
    X86Instr* p = malloc(sizeof(X86Instr));
#ifdef LOG_MEM
    fprintf(stderr, "%s: Allocated X86Instr %p\n", __func__, (void*)p);
#endif
    x86_instr_init(p, opcode);
    return p;
}

void x86_instr_link(X86Instr* i1, X86Instr* i2) {
    if(i1->next != NULL) {
        fprintf(stderr, "%s: i1 (%p) already has a next.\n", __func__, (void*)i1);
    }
    if(i2->prev != NULL) {
        fprintf(stderr, "%s: i2 (%p) already has a prev.\n", __func__, (void*)i2);
    }
    i1->next = i2;
    i2->prev = i1;
}

void x86_instr_clear(X86Instr* instr) {
    if(instr->prev != NULL) {
        instr->prev->next = NULL;
    }
    if(instr->next != NULL) {
        instr->next->prev = NULL;
    }
    x86_instr_init(instr, 0);
}

void x86_instr_destroy(X86Instr* instr) {
    x86_instr_clear(instr);
    free(instr);
#ifdef LOG_MEM
    fprintf(stderr, "%s: Destroyed X86Instr %p\n", __func__, (void*)p);
#endif
}

void x86_instr_destroy_list(X86Instr* n) {
    X86Instr* p = NULL;
    while(n != NULL) {
        p = n->next;
        x86_instr_destroy(n);
        n = p;
    }
}

void x86_instr_print(X86Instr* n, FILE* fp) {
    if(n->opcode == X86_OP_label) {
        fprintf(fp, "%s%s:", X86_INDENT_STR, n->arg1);
    }
    else if(n->arg2[0] != '\0') {
        fprintf(fp, "%s%s %s, %s", X86_INDENT_STR, X86_OP_STRS[n->opcode], n->arg1, n->arg2);
    }
    else if(n->arg1[0] != '\0') {
        fprintf(fp, "%s%s %s", X86_INDENT_STR, X86_OP_STRS[n->opcode], n->arg1);
    }
    else {
        fprintf(fp, "%s%s", X86_INDENT_STR, X86_OP_STRS[n->opcode]);
    }
}

#define TYPE pX86Instr
#define TYPED(x) x86_instr_##x
#define CLASS X86Code
#define CLASSED(x) x86_code_##x
#include "util/llist.gen.c"
#undef TYPE
#undef TYPED
#undef CLASS
#undef CLASSED

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "ircode.h"

void irinstr_init(IRInstr* instr, op_t op) {
    instr->op = op;
    instr->arg1 = NULL;
    instr->arg2 = NULL;
    instr->res = NULL;
    instr->prev = NULL;
    instr->next = NULL;
}

IRInstr* irinstr_new(op_t op) {
    IRInstr* p = malloc(sizeof(IRInstr));
#ifdef LOG_MEM
    fprintf(stderr, "%s: Allocated IRInstr %p\n", __func__, (void*)p);
#endif
    irinstr_init(p, op);
    return p;
}

void irinstr_link(IRInstr* i1, IRInstr* i2) {
    if(i1->next != NULL) {
        fprintf(stderr, "%s: i1 (%p) already has a next.\n", __func__, (void*)i1);
    }
    if(i2->prev != NULL) {
        fprintf(stderr, "%s: i2 (%p) already has a prev.\n", __func__, (void*)i2);
    }
    i1->next = i2;
    i2->prev = i1;
}

void irinstr_clear(IRInstr* instr) {
    if(instr->prev != NULL) {
        instr->prev->next = NULL;
    }
    if(instr->next != NULL) {
        instr->next->prev = NULL;
    }
    irinstr_init(instr, 0);
}

void irinstr_destroy(IRInstr* instr) {
    irinstr_clear(instr);
    free(instr);
#ifdef LOG_MEM
    fprintf(stderr, "%s: Destroyed IRInstr %p\n", __func__, (void*)instr);
#endif
}

void irinstr_destroy_list(IRInstr* n) {
    IRInstr* p = NULL;
    while(n != NULL) {
        p = n->next;
        irinstr_destroy(n);
        n = p;
    }
}

void irinstr_print(IRInstr* n, FILE* fp) {
    fprintf(fp, "Instr(op=%s", OP_STRS[n->op]);
    fprintf(fp, ",\n\tres=");
    AddrNode_print(n->res, fp);
    fprintf(fp, ",\n\targ1=");
    AddrNode_print(n->arg1, fp);
    fprintf(fp, ",\n\targ2=");
    AddrNode_print(n->arg2, fp);
    fprintf(fp, ")");
}

#define TYPE pIRInstr
#define TYPED(x) irinstr_##x
#define CLASS IRCode
#define CLASSED(x) ircode_##x
#include "util/llist.gen.c"
#undef TYPE
#undef TYPED
#undef CLASS
#undef CLASSED

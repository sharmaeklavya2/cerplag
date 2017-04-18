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

void ircode_init(IRCode* irc) {
    irc->first = NULL;
    irc->last = NULL;
    irc->size = 0;
}

void ircode_copy(IRCode* irc, IRCode* irc2) {
    irc->first = irc2->first;
    irc->last = irc2->last;
    irc->size = irc2->size;
}

void ircode_combine(IRCode* irc, IRCode* p1, IRCode* p2) {
    irc->size = p1->size + p2->size;
    if(p1->first == NULL) {
        irc->first = p2->first;
        irc->last = p2->last;
    }
    else if(p2->first == NULL) {
        irc->first = p1->first;
        irc->last = p1->last;
    }
    else {
        irinstr_link(p1->last, p2->first);
        irc->first = p1->first;
        irc->last = p2->last;
    }
}

void ircode_prepend(IRCode* irc, IRInstr* instr) {
    (irc->size)++;
    if(irc->first == NULL) {
        irc->first = irc->last = instr;
        irc->size = 1;
    }
    else {
        irinstr_link(instr, irc->first);
        irc->first = instr;
    }
}

void ircode_append(IRCode* irc, IRInstr* instr) {
    (irc->size)++;
    if(irc->first == NULL) {
        irc->first = irc->last = instr;
        irc->size = 1;
    }
    else {
        irinstr_link(irc->last, instr);
        irc->last = instr;
    }
}

void ircode_clear(IRCode* irc) {
    irinstr_destroy_list(irc->first);
    irc->first = irc->last = NULL;
    irc->size = 0;
}

void ircode_print(IRCode* irc, FILE* fp) {
    int i;
    IRInstr* node = irc->first;
    for(i=0; i<(irc->size); ++i) {
        irinstr_print(node, fp);
        fprintf(fp, "\n");
        node = node->next;
    }
}

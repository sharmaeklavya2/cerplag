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

void ircode_init(IRCode* irc) {
    irc->first = NULL;
    irc->last = NULL;
    irc->size = 0;
}

void ircode_combine(IRCode* irc, IRCode* p1, IRCode* p2) {
    irc->size = p1->size + p2->size;
    if(p1->first == NULL) {
        irc->first = p2->first;
        irc->last = p2->last;
        irc->size = p2->size;
    }
    else if(p2->first == NULL) {
        irc->first = p1->first;
        irc->last = p1->last;
        irc->size = p1->size;
    }
    else {
        irc->first = p1->first;
        irc->last = p2->last;
        irinstr_link(p1->last, p2->first);
    }
}

void ircode_add(IRCode* irc, IRInstr* instr) {
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

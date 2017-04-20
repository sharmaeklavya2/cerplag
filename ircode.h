#ifndef H_IRCODE
#define H_IRCODE

#include "ircode_defs.h"

void irinstr_init(IRInstr* instr, op_t op);
void irinstr_init2(IRInstr* instr, op_t op, AddrNode* res, AddrNode* arg1, AddrNode* arg2);
IRInstr* irinstr_new(op_t op);
IRInstr* irinstr_new2(op_t op, AddrNode* res, AddrNode* arg1, AddrNode* arg2);
void irinstr_link(IRInstr* i1, IRInstr* i2);
void irinstr_clear(IRInstr* instr);
void irinstr_destroy(IRInstr* instr);
void irinstr_destroy_list(IRInstr* n);
void irinstr_print(IRInstr* n, FILE* fp);

void ircode_init(IRCode* irc);
void ircode_copy(IRCode* irc, IRCode* irc2);
void ircode_combine(IRCode* irc, IRCode* p1, IRCode* p2);
void ircode_prepend(IRCode* irc, IRInstr* instr);
void ircode_append(IRCode* irc, IRInstr* instr);
void ircode_clear(IRCode* irc);
void ircode_print(IRCode* irc, FILE* fp);

#endif  // H_IRCODE

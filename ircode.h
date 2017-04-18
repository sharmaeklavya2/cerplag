#ifndef H_IRCODE
#define H_IRCODE

#include "ircode_defs.h"

void irinstr_init(IRInstr* instr, op_t op);
IRInstr* irinstr_new(op_t op);
void irinstr_link(IRInstr* i1, IRInstr* i2);

void ircode_init(IRCode* irc);
void ircode_combine(IRCode* irc, IRCode* p1, IRCode* p2);
void ircode_add(IRCode* irc, IRInstr* instr);

#endif  // H_IRCODE

#ifndef H_X86
#define H_X86

#include <stdio.h>
#include "x86_defs.h"

void x86_instr_init(X86Instr*, x86_op_t);
X86Instr* x86_instr_new(x86_op_t);
void x86_instr_link(X86Instr*, X86Instr*);
void x86_instr_clear(X86Instr*);
void x86_instr_destroy(X86Instr*);
void x86_instr_destroy_list(X86Instr*);
void x86_instr_print(X86Instr*, FILE*);

#endif  // H_X86

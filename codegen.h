#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "ast_defs.h"

typedef enum {
#define X(a) ##a
#include "data/opcode.xmac"
#undef X
	NOP
} opcode_t;

//typedef int opcode_t;
typedef int addr_t;

typedef struct quadruple {
	opcode_t op;
	addr_t arg1;
	addr_t arg2;
	addr_t res;
	struct quadruple* next;
}quadruple;

typedef struct q_list {
	quadruple * first;
	quadruple * last;
}q_list_t;

#endif

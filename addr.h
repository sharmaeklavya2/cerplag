#ifndef H_ADDR
#define H_ADDR

#include <stdbool.h>
#include <stdlib.h>
#include "addr_defs.h"

void AddrNode_destroy(AddrNode*);
void AddrNode_print(const AddrNode*, FILE*);

void AddrList_init(AddrList*);
void AddrList_clear(AddrList*);
void AddrList_add(AddrList*, AddrNode*);
void AddrList_print(const AddrList*, FILE*);

#endif  // H_ADDR

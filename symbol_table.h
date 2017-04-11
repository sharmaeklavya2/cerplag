#ifndef H_SYMBOL_TABLE
#define H_SYMBOL_TABLE

#include "symbol_table_defs.h"

void ST_reinit(STStack* psts, const char* func_name);
void ST_add_entry(STStack* psts, pSTEntry pentry);
pSTEntry ST_get_entry(STStack* psts, const char* lexeme);
void ST_add_table(STStack*);
void ST_remove_table(STStack*);
void STStack_destroy(STStack*);

#endif  // H_SYMBOL_TABLE

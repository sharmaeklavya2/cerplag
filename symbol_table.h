#ifndef H_SYMBOL_TABLE
#define H_SYMBOL_TABLE

#include "symbol_table_defs.h"

int get_type_width(valtype_t type, int size);
void pSTEntry_print(pSTEntry p, FILE* fp);
void pSTEntry_print_sub(pSTEntry p, FILE* fp);

void SD_init(pSD psd);
bool SD_add_entry(pSD psd, pSTEntry pentry);
pSTEntry SD_get_entry(pSD psd, const char* lexeme);
pST SD_get_subroot(pSD psd);
void SD_add_scope(pSD psd, pAstNode scope);
void SD_remove_scope(pSD psd);
void SD_clear(pSD psd);
void SD_print(pSD psd, FILE* fp);

#endif  // H_SYMBOL_TABLE

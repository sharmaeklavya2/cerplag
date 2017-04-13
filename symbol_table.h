#ifndef H_SYMBOL_TABLE
#define H_SYMBOL_TABLE

#include "symbol_table_defs.h"

void SD_init(pSD psd);
void SD_add_entry(pSD psd, pSTEntry pentry);
pSTEntry SD_get_entry(pSD psd, const char* lexeme);
pST SD_get_root(pSD psd);
void SD_add_scope(pSD psd, pAstNode scope);
void SD_remove_scope(pSD psd);
void SD_clear(pSD psd);

#endif  // H_SYMBOL_TABLE

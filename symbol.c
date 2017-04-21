#include <stdio.h>
#include "symbol_defs.h"

char* GS_STRS[] = {
#define X(a, b) #a,
#include "tok.xmac"
#undef X
#define X(a) #a,
#include "nonterms.xmac"
#undef X
};

void psymbol_print(Symbol* p, FILE* fp) {
    const char* lexeme = (p->lexeme == NULL) ? "NULL" : p->lexeme;
    fprintf(fp, "Symbol(%s, %s, %d:%d, rule%d)", lexeme, GS_STRS[p->sid], p->line, p->col, p->rule_num);
}

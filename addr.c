#include "addr.h"

char* ADDR_TYPE_STRS[] = ADDR_TYPE_STRS_LIST;

void AddrNode_destroy(AddrNode* an) {
    free(an);
}

void AddrList_init(AddrList* al) {
    al->first = al->last = NULL;
    al->size = 0;
}

void AddrList_clear(AddrList* al) {
    AddrNode* an = al->first;
    AddrNode* tn2 = NULL;
    for(; an != NULL; an = tn2) {
        tn2 = an->next;
        AddrNode_destroy(an);
    }
    al->first = al->last = NULL;
    al->size = 0;
}

void AddrList_add(AddrList* al, AddrNode* an) {
    if(an != NULL) {
        (al->size)++;
        if(al->first == NULL) {
            al->first = an;
        }
        else {
            al->last->next = an;
        }
        al->last = an;
    }
}

void AddrNode_print(const AddrNode* an, FILE* fp) {
    fprintf(fp, "AddrNode(%d, %d:%d, %s %s", an->id, an->line, an->col, ADDR_TYPE_STRS[an->addr_type],
        TYPE_STRS[an->type]);
    if(an->size > 0) {
        fprintf(fp, "[%d]", an->size);
    }
    fprintf(fp, ", %d)", an->offset);
}

void AddrList_print(const AddrList* al, FILE* fp) {
    AddrNode* an = al->first;
    for(; an != NULL; an = an->next) {
        AddrNode_print(an, fp);
        fprintf(fp, "\n");
    }
}


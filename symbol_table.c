#include <stdlib.h>
#include "symbol_table.h"
#include "error.h"
#include "ast.h"

void pSTEntry_destroy(pSTEntry p)
{
#ifdef LOG_MEM
    fprintf(stderr, "Called %s(%p)\n", __func__, (void*)p);
#endif
    free(p);
}

void pSTEntry_print(pSTEntry p, FILE* fp)
{
    fprintf(fp, "STEntry(%s: %s", p->lexeme, TYPE_STRS[p->type]);
    if(p->size > 0)
        fprintf(fp, "[%d]", p->size);
    fprintf(fp, ", %d:%d, %d)", p->line, p->col, p->offset);
}

#define KTYPE vptr
#define VTYPE pSTEntry
#define KTYPED(x) vptr_##x
#define VTYPED(x) pSTEntry_##x
#define ITYPED(x) ST_##x
#include "util/hmap.gen.c"
#undef KTYPE
#undef VTYPE
#undef KTYPED
#undef VTYPED
#undef ITYPED

void ST_print(pST p, FILE* fp)
{ST_hmap_print(&(p->vmap), fp);}

void ST_clear(pST p)
{ST_hmap_clear(&(p->vmap));}

void ST_destroy(pST p) {
#ifdef LOG_MEM
    fprintf(stderr, "Called %s(%p)\n", __func__, (void*)p);
#endif
    ST_clear(p);
    free(p);
}

#define TYPE pST
#define TYPED(x) ST_##x
#include "util/stack.gen.c"
#undef TYPED
#undef TYPE

void SD_init(pSD psd) {
    ST_stack_clear(&(psd->st_stack));
    psd->offset = 0;
}

void SD_add_entry(pSD psd, pSTEntry pentry) {
    ST_stack_node* n = (psd->st_stack).top;
    if(n == NULL) {
        fprintf(stderr, "%s: No SymbolTable present in stack.\n", __func__);
        return;
    }
    pST pst = n->value;
    int old_size = pst->vmap.size;
    ST_hmap_node* node = ST_hmap_insert(&(pst->vmap), pentry->lexeme, pentry);
    if(pst->vmap.size == old_size) {
        char msg[80];
        sprintf(msg, "Variable has already been declared at line %d col %d.", node->value->line, node->value->col);
        print_error("compile", ERROR, 5, pentry->line, pentry->col, pentry->lexeme, "ALREADY_DECL", msg);
    }

    int offset = psd->offset;
    int align_size = TYPE_ALIGNS[pentry->type];
    int t = offset % align_size;
    if(t != 0)
        offset += t;
    pentry->offset = offset;

    if(pentry->size == 0)
        offset += TYPE_SIZES[pentry->type];
    else
        offset += TYPE_SIZES[pentry->type] * (pentry->size);
    psd->offset = offset;
}

pSTEntry SD_get_entry(pSD psd, const char* lexeme) {
    ST_stack_node* n = (psd->st_stack).top;
    ST_hmap_node* res = NULL;
    while(n != NULL && res == NULL) {
        res = ST_hmap_query(&(n->value->vmap), lexeme);
        n = n->next;
    }
    if(res == NULL)
        return NULL;
    else
        return res->value;
}

pST SD_get_root(pSD psd) {
    ST_stack_node *p = NULL, *n = (psd->st_stack).top;
    while(n != NULL) {
        p = n;
        n = n->next;
    }
    return p->value;
}

void SD_add_scope(pSD psd, pAstNode scope) {
    pST pst = malloc(sizeof(ST));
#ifdef LOG_MEM
    fprintf(stderr, "%s: Allocated ST %p\n", __func__, (void*)phmap);
#endif
    ST_hmap_init(&(pst->vmap), 10, false);
    pst->scope = scope;
    ST_stack_push(&(psd->st_stack), pst);
}

void SD_remove_scope(pSD psd) {
    ST_destroy(ST_stack_pop(&(psd->st_stack)));
}

void SD_clear(pSD psd) {
    ST_stack_clear(&(psd->st_stack));
}

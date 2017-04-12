#include <stdlib.h>
#include "symbol_table.h"
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

#define TYPE pST_hmap
#define TYPED(x) ST_hmap_##x
#include "util/stack.gen.c"
#undef TYPED
#undef TYPE

void ST_reinit(STStack* psts, const char* func_name) {
    int_stack_clear(&(psts->offsets));
    int_stack_push(&(psts->offsets), 0);

    ST_hmap_stack_clear(&(psts->map_stack));
    psts->func_name = func_name;
    ST_hmap* phmap = malloc(sizeof(ST_hmap));
#ifdef LOG_MEM
    fprintf(stderr, "%s: Allocated ST_hmap %p\n", __func__, (void*)phmap);
#endif
    ST_hmap_init(phmap, 10, false);
    ST_hmap_stack_push(&(psts->map_stack), phmap);
}

void ST_add_entry(STStack* psts, pSTEntry pentry) {
    ST_hmap_update(ST_hmap_stack_top(&(psts->map_stack)), pentry->lexeme, pentry);

    int offset = int_stack_top(&(psts->offsets));
    int align_size = TYPE_ALIGNS[pentry->type];
    int t = offset % align_size;
    if(t != 0)
        offset += t;
    pentry->offset = offset;

    if(pentry->size == 0)
        offset += TYPE_SIZES[pentry->type];
    else
        offset += TYPE_SIZES[pentry->type] * (pentry->size);
    (psts->offsets).top->value = offset;
    if(offset > (psts->max_offset))
        psts->max_offset = offset;
}

pSTEntry ST_get_entry(STStack* psts, const char* lexeme) {
    ST_hmap_stack_node* n = (psts->map_stack).top;
    ST_hmap_node* res = NULL;
    while(n != NULL && res == NULL) {
        res = ST_hmap_query(n->value, lexeme);
        n = n->next;
    }
    if(res == NULL)
        return NULL;
    else
        return res->value;
}

void ST_add_table(STStack* psts) {
    ST_hmap* phmap = malloc(sizeof(ST_hmap));
#ifdef LOG_MEM
    fprintf(stderr, "%s: Allocated ST_hmap %p\n", __func__, (void*)phmap);
#endif
    ST_hmap_init(phmap, 10, false);
    ST_hmap_stack_push(&(psts->map_stack), phmap);

    int_stack_push(&(psts->offsets), int_stack_top(&(psts->offsets)));
}

void ST_remove_table(STStack* psts) {
    ST_hmap_destroy(ST_hmap_stack_pop(&(psts->map_stack)));
    int_stack_pop(&(psts->offsets));
}

void STStack_clear(STStack* psts) {
    ST_hmap_stack_clear(&(psts->map_stack));
    int_stack_clear(&(psts->offsets));
}

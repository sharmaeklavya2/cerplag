#include <stdlib.h>
#include "symbol_table.h"
#include "error.h"
#include "ast.h"

int get_type_width(valtype_t type, int size) {
    if(size == 0) {
        return TYPE_SIZES[type];
    }
    else {
        return TYPE_SIZES[type] * size;
    }
}

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

void pSTEntry_print_sub(pSTEntry p, FILE* fp) {
    char type_str[10] = "";
    if(p->size == 0)
        sprintf(type_str, "%s", TYPE_STRS[p->type]);
    else
        sprintf(type_str, "%s[%d]", TYPE_STRS[p->type], p->size);
    int beg_line = get_scope_beg_line(p->symbol_table->scope);
    int end_line = get_scope_end_line(p->symbol_table->scope);
    const char* func_name = p->func_name == NULL ? "driver" : p->func_name;
    fprintf(fp, "%-10s %-12s %-10s %3d to %3d %6d %6d %7d\n", p->lexeme, type_str, func_name, beg_line, end_line,
        p->symbol_table->level, get_type_width(p->type, p->size), p->offset);
    //pSTEntry_print(p, fp);
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
#include "util/tree.gen.c"
#undef TYPED
#undef TYPE

void SD_init(pSD psd) {
    ST_tree_destroy(psd->root);
    psd->level = 0;
    psd->offset = 0;
#ifdef LOG_MEM
    fprintf(stderr, "Called %s(%p)\n", __func__, (void*)psd);
#endif

    pST pst = malloc(sizeof(ST));
#ifdef LOG_MEM
    fprintf(stderr, "%s: Allocated ST %p\n", __func__, (void*)pst);
#endif
    ST_hmap_init(&(pst->vmap), 10, false);
    pst->scope = NULL;
    pst->level = 0;
    psd->root = psd->active = ST_tree_get_node(pst);
}

bool SD_add_entry(pSD psd, pSTEntry pentry) {
    if(psd->active == NULL) {
        fprintf(stderr, "%s: No SymbolTable present in stack.\n", __func__);
        return false;
    }
    pST pst = psd->active->value;

    int old_size = pst->vmap.size;
    ST_hmap_node* node = ST_hmap_insert(&(pst->vmap), pentry->lexeme, pentry);
    if(pst->vmap.size == old_size) {
        char msg[80];
        sprintf(msg, "Variable has already been declared at line %d col %d.", node->value->line, node->value->col);
        print_error("compile", ERROR, 5, pentry->line, pentry->col, pentry->lexeme, "ALREADY_DECL", msg);
        return false;
    }

    int offset = psd->offset;
    int align_size = TYPE_ALIGNS[pentry->type];
    int t = offset % align_size;
    if(t != 0)
        offset += t;
    pentry->offset = offset;

    offset += get_type_width(pentry->type, pentry->size);
    psd->offset = offset;
    return true;
}

pSTEntry SD_get_entry(pSD psd, const char* lexeme) {
    ST_tree_node* n = psd->active;
    ST_hmap_node* res = NULL;
    while(n != NULL && res == NULL) {
        res = ST_hmap_query(&(n->value->vmap), lexeme);
        n = n->parent;
    }
    if(res == NULL)
        return NULL;
    else
        return res->value;
}

pST SD_get_subroot(pSD psd) {
    // doesn't actually return root of symbol database.
    // returns level 1 node under which the active node lies.
    return psd->root->last_child->value;
}

void SD_add_scope(pSD psd, pAstNode scope) {
    pST pst = malloc(sizeof(ST));
#ifdef LOG_MEM
    fprintf(stderr, "%s: Allocated ST %p\n", __func__, (void*)pst);
#endif
    ST_hmap_init(&(pst->vmap), 10, false);
    pst->scope = scope;
    pst->level = (++psd->level);
    ST_tree_insert(psd->active, pst);
    psd->active = psd->active->last_child;
}

void SD_remove_scope(pSD psd) {
    psd->active = psd->active->parent;
    psd->level--;
    if(psd->active->parent == NULL) {   // if root is active
        psd->offset = 0;
    }
}

void SD_clear(pSD psd) {
#ifdef LOG_MEM
    fprintf(stderr, "Called %s(%p)\n", __func__, (void*)psd);
#endif
    ST_tree_destroy(psd->root);
    psd->root = psd->active = NULL;
    psd->level = 0;
    psd->offset = 0;
}

void SD_print(pSD psd, FILE* fp) {
    ST_tree_print(psd->root->first_child, fp);
}

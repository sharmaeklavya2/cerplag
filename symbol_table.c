#include <stdlib.h>
#include "symbol_table.h"
#include "error.h"
#include "ast.h"
#include "type.h"

void pSTEntry_destroy(pSTEntry p)
{
#ifdef LOG_MEM
    fprintf(stderr, "Called %s(%p)\n", __func__, (void*)p);
#endif
    free(p);
}

void pSTEntry_print(pSTEntry p, FILE* fp)
{
    fprintf(fp, "STEntry(%s: %s", p->lexeme, TYPE_STRS[p->addr->type]);
    if(p->addr->size > 0)
        fprintf(fp, "[%d]", p->addr->size);
    fprintf(fp, ", %d:%d, %d)", p->addr->line, p->addr->col, p->addr->offset);
}

void pSTEntry_print_sub(pSTEntry p, FILE* fp) {
    char type_str[10] = "";
    if(p->addr->size == 0)
        sprintf(type_str, "%s", TYPE_STRS[p->addr->type]);
    else
        sprintf(type_str, "%s[%d]", TYPE_STRS[p->addr->type], p->addr->size);
    int beg_line = p->addr->symbol_table->scope_beg_line;
    int end_line = p->addr->symbol_table->scope_end_line;
    const char* func_name = p->func_name == NULL ? "driver" : p->func_name;
    fprintf(fp, "%-10s %-12s %-10s %3d to %3d %6d %6d %7d\n", p->lexeme, type_str, func_name, beg_line, end_line,
        p->addr->symbol_table->level, get_type_width(p->addr->type, p->addr->size), p->addr->offset);
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

void ST_init(pST pst, struct AstNode* scope, int level) {
    ST_hmap_init(&(pst->vmap), 10, false);
    AddrList_init(&(pst->addrs));
    pst->scope = scope;
    pst->scope_beg_line = get_scope_beg_line(scope);
    pst->scope_end_line = get_scope_end_line(scope);
    pst->beg_offset = pst->end_offset = pst->max_offset = 0;
    pst->level = level;
}

void ST_print(pST p, FILE* fp)
{
    fprintf(fp, "SymbolTable(off=%d:%d, maxoff=%d):\n", p->beg_offset, p->end_offset, p->max_offset);
    ST_hmap_print(&(p->vmap), fp);
    fprintf(fp, "AddrList:\n");
    AddrList_print(&(p->addrs), fp);
}

void ST_clear(pST p)
{
    ST_hmap_clear(&(p->vmap));
    AddrList_clear(&(p->addrs));
    p->scope = NULL;
    p->level = 0;
    p->beg_offset = p->end_offset = p->max_offset = 0;
}

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
    psd->level = 0;
    psd->offset = 0;
#ifdef LOG_MEM
    fprintf(stderr, "Called %s(%p)\n", __func__, (void*)psd);
#endif

    pST pst = malloc(sizeof(ST));
#ifdef LOG_MEM
    fprintf(stderr, "%s: Allocated ST %p\n", __func__, (void*)pst);
#endif
    ST_init(pst, NULL, 0);
    psd->root = psd->active = ST_tree_get_node(pst);
    psd->first_entry = psd->last_entry = NULL;
}

bool SD_add_entry(pSD psd, pSTEntry pentry, int line, int col) {
    // pentry->addr is not yet initialized
    if(psd->active == NULL) {
        fprintf(stderr, "%s: No SymbolTable present in stack.\n", __func__);
        return false;
    }
    pST pst = psd->active->value;

    int old_size = pst->vmap.size;
    ST_hmap_node* node = ST_hmap_insert(&(pst->vmap), pentry->lexeme, pentry);
    if(pst->vmap.size == old_size) {
        char msg[80];
        sprintf(msg, "Variable has already been declared at line %d col %d.", node->value->addr->line, node->value->addr->col);
        print_error("semantic", ERROR, -1, line, col, pentry->lexeme, "ALREADY_DECL", msg);
        return false;
    }

    if(psd->first_entry == NULL) {
        psd->first_entry = psd->last_entry = pentry;
    }
    else {
        psd->last_entry->next = pentry;
        psd->last_entry = pentry;
    }
    return true;
}

bool SD_add_addr(pSD psd, AddrNode* an) {
    if(psd->active == NULL) {
        fprintf(stderr, "%s: No SymbolTable present in stack.\n", __func__);
        return false;
    }
    pST pst = psd->active->value;

    if(pst->addrs.last == NULL) {
        an->id = 0;
    }
    else {
        an->id = pst->addrs.last->id + 1;
    }

    AddrList_add(&(pst->addrs), an);

    if(an->addr_type != ADDR_CONST) {
        an->offset = get_aligned_offset(psd->offset, an->type);
        psd->offset = an->offset + get_type_width(an->type, an->size);
        if((psd->offset) > (pst->max_offset)) {
            pst->max_offset = psd->offset;
        }
    }
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
    ST_init(pst, scope, ++(psd->level));
    ST_tree_insert(psd->active, pst);
    psd->active = psd->active->last_child;
}

void SD_remove_scope(pSD psd) {
    ST_tree_node* parent = psd->active->parent;
    if(parent != NULL) {
        int pmo = parent->value->max_offset;
        int mo = psd->active->value->max_offset;
        if(mo > pmo) {pmo = mo;}
        parent->value->max_offset = pmo;
    }
    psd->active = parent;
    psd->level--;
    if(parent->parent == NULL) {   // if now root is active
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
    psd->first_entry = psd->last_entry = NULL;
}

void SD_print(pSD psd, FILE* fp) {
    ST_tree_print(psd->root, fp);
}
void SD_print_sub(pSD psd, FILE* fp) {
    fprintf(stderr, "%-10s %-12s %-10s %-10s %6s %6s %7s\n",
        "lexeme", "type", "scope", "lines", "level", "width", "offset");
    for(pSTEntry node = psd->first_entry; node != NULL; node = node->next) {
        pSTEntry_print_sub(node, stderr);
    }
}

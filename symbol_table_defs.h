#ifndef H_SYMBOL_TABLE_DEFS
#define H_SYMBOL_TABLE_DEFS

#include <stdio.h>
#include "ast.h"
#include "util/vptr.h"
#include "util/int_stack.h"

struct SymbolTable;
struct AddrNode;
struct AddrList;

typedef struct STEntry {
// Symbol Table Entry: an entry in the variable map
    const char* lexeme;
    const char* func_name;
    int use_line;   // line number of last use
    int use_col;    // col number of last use
    bool readonly;
    struct STEntry* next;
    struct AddrNode* addr;
} STEntry;  // Symbol Table Entry

typedef STEntry* pSTEntry;

#define KTYPE vptr
#define VTYPE pSTEntry
#define KTYPED(x) vptr_##x
#define VTYPED(x) pSTEntry_##x
#define ITYPED(x) ST_##x
#include "util/hmap.gen.h"
#undef KTYPE
#undef VTYPE
#undef KTYPED
#undef VTYPED
#undef ITYPED

typedef struct SymbolTable {
    ST_hmap vmap;
    struct AddrList addrs;
    pAstNode scope;
    int level;  // depth of node in tree
} SymbolTable;

typedef SymbolTable ST;
typedef ST* pST;

#define TYPE pST
#define TYPED(x) ST_##x
#include "util/tree.gen.h"
#undef TYPED
#undef TYPE

typedef struct SymbolDatabase {
    ST_tree_node* root;
    ST_tree_node* active;
    int level;  // depth of the active node
    int offset;
}SymbolDatabase;

typedef SymbolDatabase SD;
typedef SD* pSD;

#include "addr_defs.h"

#endif  // H_SYMBOL_TABLE_DEFS

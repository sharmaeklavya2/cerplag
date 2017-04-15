#ifndef H_SYMBOL_TABLE_DEFS
#define H_SYMBOL_TABLE_DEFS

#include <stdio.h>
#include "ast.h"
#include "util/vptr.h"
#include "util/int_stack.h"

struct SymbolTable;

typedef struct STEntry {
    const char* lexeme;
    const char* func_name;
    valtype_t type; // type of variable
    int size;       // size of variable if it is an array, 0 otherwise
    int line;       // line number of definition
    int col;        // col number of definition
    int use_line;   // line number of last use
    int use_col;    // col number of last use
    int offset;     // offset in activation record
    bool readonly;
    struct SymbolTable* symbol_table;
    struct STEntry* next;
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

#endif  // H_SYMBOL_TABLE_DEFS

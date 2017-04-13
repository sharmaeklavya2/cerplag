#ifndef H_SYMBOL_TABLE_DEFS
#define H_SYMBOL_TABLE_DEFS

#include <stdio.h>
#include "ast.h"
#include "util/vptr.h"
#include "util/int_stack.h"

typedef struct {
    const char* lexeme;
    valtype_t type; // type of variable
    int size;       // size of variable if it is an array, 0 otherwise
    int line;       // line number of definition
    int col;        // col number of definition
    int offset;     // offset in activation record
    bool readonly;
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
} SymbolTable;

typedef SymbolTable ST;
typedef ST* pST;

#define TYPE pST
#define TYPED(x) ST_##x
#include "util/stack.gen.h"
#undef TYPED
#undef TYPE

typedef struct SymbolDatabase {
    ST_stack st_stack;
    int offset;
}SymbolDatabase;

typedef SymbolDatabase SD;
typedef SD* pSD;

#endif  // H_SYMBOL_TABLE_DEFS

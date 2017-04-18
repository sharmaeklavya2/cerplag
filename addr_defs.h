#ifndef H_ADDR_DEFS
#define H_ADDR_DEFS

#include "type.h"

struct SymbolTable;

typedef enum {ADDR_VAR, ADDR_TEMP, ADDR_CONST, ADDR_ARR} addr_type_t;

#define ADDR_TYPE_STRS_LIST {"var", "temp", "const", "arr"}

typedef struct AddrNode {
// Stores information about an Intermediate Representation variable
    struct AddrNode* next;
    struct SymbolTable* symbol_table;
    int id;
    addr_type_t addr_type;
    valtype_t type; // type of variable
    int size;       // number of elements in variable if it is an array, 0 otherwise
                    // size should always be 0 for anything other than ADDR_VAR
    int line;       // line number of creation
    int col;        // col number of creation
    int offset;     // offset in activation record (not used for ADDR_CONST and ADDR_ARR)
    union {
        union {
            int i;
            double f;
            bool b;
        } imm;      // immediate data - only valid for ADDR_CONST
        struct {
            struct AddrNode* base_addr;
            struct AddrNode* index;
        };          // only used for ADDR_ARR
    };
} AddrNode;

typedef struct AddrList{
    AddrNode* first;
    AddrNode* last;
    int size;
} AddrList;

#include "symbol_table.h"

#endif  // H_ADDR_DEFS

#ifndef H_SYMBOL_DEFS
#define H_SYMBOL_DEFS

#include "pch_int_hmap.h"

typedef enum
{
#define X(a, b) GS_##a,
#include "tok.xmac"
#undef X
#define X(a) GS_##a,
#include "nonterms.xmac"
#undef X
    GS_LAST
}gsymb_t;

#define NUM_GS GS_LAST

extern char* GS_STRS[];

typedef struct
{
    int line, col;
    char* lexeme;
    int sid;
    union {
        int i;
        float f;
    };
    int rule_num;

    void* tree;
    union {
        void* next;
        void* acc;
    };
    union {
        void* driver;
        struct {
            int type;
            int size;
        };
        char* varname;
        struct {
            int beg;
            int end;
        };
        int op;
    };
    int scope_beg_line;
    int scope_end_line;
}Symbol;

#endif  // H_SYMBOL_DEFS

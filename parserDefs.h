/* Batch no. 13
2014A7PS130P: Eklavya Sharma
2014A7PS023P: Daivik Nema */
#ifndef H_PARSER_DEFS
#define H_PARSER_DEFS

#include "token.h"
#include "bitset.h"
#include "pch_int_hmap.h"

/*
typedef enum
{
#define X(a) NT_##a,
#include "nonterms.xmac"
#undef X
    NT_LAST
}non_term_t;

#define NUM_NON_TERMS NT_LAST
*/

#define NUM_RULES 100

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

struct gt_node_
{
    struct gt_node_* next;
    gsymb_t value;
    bitset_t first;
};

typedef struct gt_node_ gt_node;

typedef struct
{
    gsymb_t lhs;
    gt_node* head;
    gt_node* tail;
}Rule;

extern pch_int_hmap intern_table;

typedef Token Symbol;

#endif  // H_PARSER_DEFS

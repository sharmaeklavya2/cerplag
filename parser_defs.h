#ifndef H_PARSER_DEFS
#define H_PARSER_DEFS

/*
typedef enum
{
#define X(a) NT_##a,
#include "data/nonterms.xmac"
#undef X
    NT_LAST
}non_term_t;

#define NUM_NON_TERMS NT_LAST
*/

#define NUM_RULES 100

typedef enum
{
#define X(a, b) GS_##a,
#include "data/tok.xmac"
#undef X
#define X(a) GS_##a,
#include "data/nonterms.xmac"
#undef X
    GS_LAST
}gsymb_t;

#define NUM_GS GS_LAST

struct gt_node_
{
    struct gt_node_* next;
    gsymb_t value;
};

typedef struct gt_node_ gt_node;

typedef struct
{
    gsymb_t lhs;
    gt_node* head;
    gt_node* tail;
}Rule;

#endif  // H_PARSER_DEFS

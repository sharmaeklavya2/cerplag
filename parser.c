#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "parser_defs.h"
#include "util/pch_int_hmap.h"

static pch_int_hmap gsymb_ht;

static char* GS_STRS[] = {
#define X(a, b) #a,
#include "data/tok.xmac"
#undef X
#define X(a) #a,
#include "data/nonterms.xmac"
#undef X
};

void init_gsymb_ht()
{
    pch_int_hmap_init(&gsymb_ht, 150);
#define X(a, b) pch_int_hmap_update(&gsymb_ht, #a, GS_##a);
#include "data/tok.xmac"
#undef X
#define X(a) pch_int_hmap_update(&gsymb_ht, #a, GS_##a);
#include "data/nonterms.xmac"
#undef X
}

Rule rules[NUM_RULES];

gt_node* get_gt_node(gsymb_t sid)
{
    gt_node* n = malloc(sizeof(gt_node));
    n->value = sid;
    n->next = NULL;
    return n;
}

gsymb_t get_gsymb_id(const char* str)
{
    pch_int_hmap_node* hmap_n = pch_int_hmap_query(&gsymb_ht, str);
    if(hmap_n == NULL)
    {
        fprintf(stderr, "Unrecognized grammar symbol %s\n", str);
        return GS_LAST;
    }
    return hmap_n->value;
}
    
void read_grammar(FILE* fp)
{
    int i=0;
    bool lhs = true;
    char str[100];
    while(fscanf(fp, "%s", str) != EOF)
    {
        if(str[0] == '.')
        {
            i++;
            lhs = true;
            continue;
        }
        gsymb_t sid = get_gsymb_id(str);
        if(lhs)
        {
            rules[i].lhs = sid;
            lhs = false;
        }
        else
        {
            gt_node* n = get_gt_node(sid);
            if(rules[i].head == NULL)
                rules[i].head = n;
            else
                rules[i].tail->next = n;
            rules[i].tail = n;
        }
    }
}

int parser_main(FILE* ifp, FILE* ofp, int verbosity)
{
    init_gsymb_ht();
    char str[100];
    FILE* gfp = fopen("data/grammar.txt", "r");
    fscanf(gfp, "%s", str);
    gsymb_t start_symb = get_gsymb_id(str);
    fprintf(stderr, "start_symbol = %s\n", GS_STRS[start_symb]);
    read_grammar(gfp);
    return 0;
}

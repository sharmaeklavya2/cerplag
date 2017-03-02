#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "parser_defs.h"
#include "util/pch_int_hmap.h"
#include "lexer.h"
#include "token.h"
#include "util/int_stack.h"
#include "util/tree.h"

#define NUM_NONTERMS NUM_GS - NUM_TOKENS

// Global variables ------------------------------------------------------------

static pch_int_hmap gsymb_ht;

Rule rules[NUM_RULES];  // list of rules in the grammar

int pt[NUM_NONTERMS][NUM_TOKENS];   // parse table

char* GS_STRS[] = {
#define X(a, b) #a,
#include "data/tok.xmac"
#undef X
#define X(a) #a,
#include "data/nonterms.xmac"
#undef X
};

// Utility ---------------------------------------------------------------------

bool is_t(gsymb_t x)
{ return (int)x < NUM_TOKENS; }
bool is_nt(gsymb_t x)
{ return (int)x >= NUM_TOKENS && (int)x < NUM_GS; }

int get_nt_id(gsymb_t s){
    if(s < 0)
        fprintf(stderr, "get_nt_id: got -ve input %d.\n", s);
    else if(s < (int)NUM_TOKENS)
        fprintf(stderr, "get_nt_id: got terminal input %d.\n", s);
    else if(s >= (int)NUM_GS)
        fprintf(stderr, "get_nt_id: got too big input %d.\n", s);
    else
        return s - NUM_TOKENS;
    return -1;
}

void push_ll(int_Stack* pst, gt_node * head){
    if(head == NULL) return;
    push_ll(pst, head->next);
    int_stack_push(pst, head->value);
}

void init_symbol(Symbol * symb){
    symb->line = 0;
    symb->col = 0;
    symb->lexeme = NULL;
    symb->size = 0;
    symb->tid = T_ERR;
    symb->num.f = 0.0;
}

void copy_symbol(Symbol* symb, const Token* tok)
{
    symb->line = tok->line;
    symb->col = tok->col;
    symb->tid = tok->tid;
    symb->num.f = tok->num.f;
    symb->size = tok->size;
    symb->lexeme = malloc(sizeof(char) * (tok->size));
    strcpy(symb->lexeme, tok->lexeme);
}

Symbol * make_symbol(gsymb_t sid){
    Symbol * ret = (Symbol*)malloc(sizeof(Symbol));
    init_symbol(ret);
    ret->tid = sid;
    return ret;
}

TreeNode * get_successor(TreeNode * curr){
    while(curr != NULL && curr->next_sibling == NULL)
    curr = curr->parent;
    if(curr != NULL) return curr->next_sibling;
    return NULL;
}

// Initialize ------------------------------------------------------------------

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

gt_node* get_gt_node(gsymb_t sid)
{
    gt_node* n = malloc(sizeof(gt_node));
    n->value = sid;
    n->next = NULL;
    return n;
}

void read_grammar(FILE* fp)
{
    int i;
    for(i=0; i<NUM_RULES; ++i)
    {
        rules[i].lhs = GS_ERR;
        rules[i].head = NULL;
        rules[i].tail = NULL;
    }
    i=0;
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
            rules[i].head = NULL;
            rules[i].tail = NULL;
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

void read_parse_table(const char*);

gsymb_t init_parser()
{
    init_lexer();
    init_gsymb_ht();
    char str[100];
    FILE* gfp = fopen("data/grammar.txt", "r");
    fscanf(gfp, "%s", str);
    gsymb_t start_symb = get_gsymb_id(str);
    //fprintf(stderr, "start_symbol = %s\n", GS_STRS[start_symb]);
    read_grammar(gfp);
    fclose(gfp);

    char pt_fname[] = "data/parse_table.txt";
    read_parse_table(pt_fname);

    return start_symb;
}

// Destroy ---------------------------------------------------------------------

void destroy_rules()
{
    int i;
    for(i=0; i < NUM_RULES; ++i)
    {
        rules[i].tail = NULL;
        gt_node *p = rules[i].head, *todel = NULL;
        rules[i].head = NULL;
        while(p != NULL)
        {
            todel = p;
            p = p->next;
            todel->next = NULL;
            free(todel);
        }
    }
}
void destroy_parser()
{
    destroy_lexer();
    pch_int_hmap_destroy(&gsymb_ht);
    destroy_rules();
}

// Get parse table -------------------------------------------------------------

void read_parse_table(const char* file_name){
    for(int i=0; i<NUM_NONTERMS; i++)
        for(int j=0; j<NUM_TOKENS; j++)
            pt[i][j] = -1;
    FILE * fp = fopen(file_name, "r");
    if(fp == NULL){
        fprintf(stderr, "File %s not found\n", file_name);
        return;
    }

    char nt_str[100];
    char t_str[100];
    int rule_no;
    while(fscanf(fp, "%s %s %d", nt_str, t_str, &rule_no) != EOF){
        int nt_id = get_nt_id(get_gsymb_id(nt_str));
        int t_id = get_gsymb_id(t_str);
        pt[nt_id][t_id] = rule_no;
    }
    fclose(fp);
}

// Build parse tree ------------------------------------------------------------

TreeNode* build_parse_tree(FILE * ifp, gsymb_t start_sym){

    int_Stack st;
    int_stack_init(&st);
    int_stack_push(&st, GS_EOF);
    int_stack_push(&st, start_sym);

    Token cur_tkn;
    Dfa dfa;
    init_dfa(&dfa);
    init_token(&cur_tkn);

    Symbol* symb = make_symbol(start_sym);
    TreeNode * current_tn = get_new_tree_node(symb);
    TreeNode * root = current_tn;

    get_token(ifp, &dfa, &cur_tkn, false);
    //fprintf(stderr, "%s\n", GS_STRS[cur_tkn.tid]);
    do{
    gsymb_t cur_st_top = int_stack_top(&st);
    if(is_t(cur_st_top) || cur_st_top == GS_EOF){
        int_stack_pop(&st);
        if(cur_st_top == cur_tkn.tid){
            copy_symbol(current_tn->value, &cur_tkn);
            current_tn = get_successor(current_tn);
        }else{
            fprintf(stderr, "parse_error_1: %2d %2d \'%s\' (%s)\nGot %s (\'%s\') instead of %s.\n\n",
                cur_tkn.line, cur_tkn.col, cur_tkn.lexeme, GS_STRS[cur_tkn.tid],
                GS_STRS[cur_tkn.tid], cur_tkn.lexeme, GS_STRS[cur_st_top]);
            //int_stack_print(&st, stderr);
        }
        get_token(ifp, &dfa, &cur_tkn, false);
        //fprintf(stderr, "%s\n", GS_STRS[cur_tkn.tid]);
        //int_stack_print(&st, stderr);
    }else{
        int rule_num = pt[get_nt_id(int_stack_top(&st))][cur_tkn.tid];
        //fprintf(stderr, "pt[%s, %d][%s, %d] = %d\n", GS_STRS[int_stack_top(&st)], get_nt_id(int_stack_top(&st)), GS_STRS[cur_tkn.tid], cur_tkn.tid, rule_num);
        int_stack_pop(&st);
        if(rule_num != -1){
            push_ll(&st, rules[rule_num].head);
            gt_node * tmp = rules[rule_num].head;
            while(tmp != NULL){
                Symbol * ps = make_symbol(tmp->value);
                insert_node(current_tn, ps);
                tmp = tmp->next;
            }
            if(current_tn->first_child != NULL) current_tn = current_tn->first_child;
            else current_tn = get_successor(current_tn);
        }else{
            fprintf(stderr, "parse_error_2: %2d %2d \'%s\' (%s)\n%s (\'%s\') isn't expected here.\n\n",
                cur_tkn.line, cur_tkn.col, cur_tkn.lexeme, GS_STRS[cur_tkn.tid],
                GS_STRS[cur_tkn.tid], cur_tkn.lexeme);
            //int_stack_print(&st, stderr);
        }
    }
    }while(st.size > 1);
    if(int_stack_top(&st) != GS_EOF){
        fprintf(stderr, "No valid derivation of string exists! 3\n");
        fprintf(stderr, "Current Token: %s\n", GS_STRS[cur_tkn.tid]);
        //int_stack_print(&st, stderr);
    }
    int_stack_destroy(&st);
    return root;
}

// Output ----------------------------------------------------------------------

void print_node(TreeNode* root, FILE* fp)
{
    /*
    if(root == NULL)
        fprintf(fp, "print_node: NULL\n");
    else
        fprintf(fp, "print_node: %p %s\n", root, GS_STRS[root->value->tid]);
    */
    Symbol* s = root->value;
    char empty[] = "----";
    if((int)(s->tid) < NUM_TOKENS)
        fprintf(fp, "%20s %2d %20s ", s->lexeme, s->line, GS_STRS[s->tid]);
    else
        fprintf(fp, "%20s %2s %20s ", empty, "--", empty);

    if(s->tid == T_NUM)
        fprintf(fp, "%20d ", s->num.i);
    else if(s->tid == T_RNUM)
        fprintf(fp, "%20lf ", s->num.f);
    else
        fprintf(fp, "%20s ", empty);

    if(root->parent == NULL)
        fprintf(fp, "%20s ", empty);
    else
        fprintf(fp, "%20s ", GS_STRS[root->parent->value->tid]);

    if(root->first_child == NULL)
        fprintf(fp, "yes ");
    else
        fprintf(fp, " no ");

    fprintf(fp, "%20s\n", GS_STRS[s->tid]);
}

void print_tree(TreeNode* root, FILE* fp)
{
    if(root != NULL)
    {
        TreeNode* n = root->first_child;
        if(n != NULL)
            print_tree(n, fp);
        print_node(root, fp);
        if(n != NULL)
            for(n = n->next_sibling; n != NULL; n = n->next_sibling)
                print_tree(n, fp);
    }
}

int parser_main(FILE* ifp, FILE* ofp, int verbosity)
{
    gsymb_t start_symb = init_parser();

    /*
    for(int i=0; i<NUM_NONTERMS; i++){
        for(int j=0; j<NUM_TOKENS; j++)
            printf("%d ", pt[i][j]);
    printf("\n");
    }
    */

    TreeNode* root = build_parse_tree(ifp, start_symb);
    print_tree(root, ofp);

    destroy_tree(root);
    destroy_parser();
    return 0;
}

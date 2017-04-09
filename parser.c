#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "parser_defs.h"
#include "util/pch_int_hmap.h"
#include "lexer.h"
#include "token.h"
#include "util/int_stack.h"
#include "parse_tree.h"
#include "util/bitset.h"

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

int rule_begin[NUM_NONTERMS], rule_end[NUM_GS];
// rule_begin[i] is the rule number of the first rule for the non-terminal i.
// rule_end[i] is 1 + the rule number of the last rule for the non-terminal i.

bitset_t first[NUM_NONTERMS], follow[NUM_NONTERMS];

bool follow_changed = false;
int num_rules;
int error_count;

const bitset_t unset_bitset = -1;
const bitset_t empty_bitset = 0ull;

pch_int_hmap intern_table;
int intern_counter = 0;

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

void push_ll(int_stack* pst, gt_node * head){
    if(head == NULL) return;
    push_ll(pst, head->next);
    int_stack_push(pst, head->value);
}

void init_symbol(Symbol * symb){
    symb->line = 0;
    symb->col = 0;
    symb->lexeme = NULL;
    symb->sid = T_ERR;
    symb->f = 0.0;
    symb->rule_num = -1;
    symb->tree = symb->next = symb->driver = NULL;
}

void copy_symbol(Symbol* symb, const Token* tok)
{
    symb->line = tok->line;
    symb->col = tok->col;
    symb->sid = tok->tid;
    symb->f = tok->f;
    symb->tree = symb->next = symb->driver = NULL;
    int sid = symb->sid;
    if(sid == T_ERR || sid == T_EOF) {
        symb->lexeme = NULL;
        fprintf(stderr, "copy_symbol: Bad token type assigned to a symbol\n");
    }
    else if(sid == T_ID) {
        pch_int_hmap_node* pnode = pch_int_hmap_update(&intern_table, tok->lexeme, intern_counter);
        if(pnode->key == tok->lexeme) {
            // this means the lexeme wasn't already present in intern_table
            symb->lexeme = malloc(sizeof(char) * (tok->size + 1));
            strcpy(symb->lexeme, tok->lexeme);
            pnode->key = symb->lexeme;
            intern_counter++;
            //fprintf(stderr, "new insert %p(\"%s\"): %d\n", pnode->key, pnode->key, pnode->value);
        }
        else {
            symb->lexeme = (char*)pnode->key;
            //fprintf(stderr, "already present %p(\"%s\"): %d\n", pnode->key, pnode->key, pnode->value);
        }
    }
    else if(sid == T_NUM || sid == T_RNUM) {
        symb->lexeme = NULL;
    }
    else {
        symb->lexeme = TOK_STRS2[sid];
    }
}

Symbol * make_symbol(gsymb_t sid){
    Symbol * ret = (Symbol*)malloc(sizeof(Symbol));
    init_symbol(ret);
    ret->sid = sid;
    return ret;
}

parse_tree_node * get_successor(const parse_tree_node * curr){
    while(curr != NULL && curr->next_sibling == NULL)
        curr = curr->parent;
    if(curr != NULL)
        return curr->next_sibling;
    return NULL;
}

parse_tree_node* get_first_child_or_successor(const parse_tree_node* curr)
{
    if(curr->first_child != NULL) return curr->first_child;
    else return get_successor(curr);
}

// Initialize ------------------------------------------------------------------

void init_gsymb_ht()
{
    pch_int_hmap_init(&gsymb_ht, 150, false);
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
    n->first = unset_bitset;
    return n;
}

int read_grammar(FILE* fp)
// returns the number of rules read
{
    int i, j;
    for(i=0; i<NUM_NONTERMS; ++i)
    {
        rule_begin[i] = 10000;
        rule_end[i] = 0;
    }
    for(i=0; i<NUM_RULES; ++i)
    {
        rules[i].lhs = NUM_GS;
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

            int ntsid = get_nt_id(sid);
            if(i < rule_begin[ntsid])
                rule_begin[ntsid] = i;
            if(i+1 > rule_end[ntsid])
                rule_end[ntsid] = i+1;
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
    for(j=0; j<NUM_NONTERMS; ++j)
    {
        if(rule_begin[j] > NUM_GS)
            fprintf(stderr, "rule_begin[%s, %d] = %d\n", GS_STRS[j + NUM_TOKENS], j, rule_begin[j]);
        if(rule_end[j] <= 0)
            fprintf(stderr, "rule_end[%s, %d] = %d\n", GS_STRS[j + NUM_TOKENS], j, rule_end[j]);
    }
    return i;
}

void read_parse_table(const char*);
bool make_parse_table();
void update_follow(gsymb_t start_symb);

gsymb_t init_parser()
{
    init_lexer();

    error_count = 0;

    init_gsymb_ht();
    pch_int_hmap_init(&intern_table, 50, true);
    char str[100];
    FILE* gfp = fopen("data/grammar.txt", "r");
    fscanf(gfp, "%s", str);
    gsymb_t start_symb = get_gsymb_id(str);
    //fprintf(stderr, "start_symbol = %s\n", GS_STRS[start_symb]);
    num_rules = read_grammar(gfp);
    fclose(gfp);

    int i;
    for(i=0; i < NUM_NONTERMS; ++i)
    {
        first[i] = unset_bitset;
        follow[i] = empty_bitset;
    }

    update_follow(start_symb);

    for(int i=0; i<NUM_NONTERMS; i++)
        for(int j=0; j<NUM_TOKENS; j++)
            pt[i][j] = -1;

    bool is_ll1 = make_parse_table();
    //char pt_fname[] = "data/parse_table.txt";
    //read_parse_table(pt_fname);

    if(!is_ll1)
        fprintf(stderr, "The grammar is not LL1\n");

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
    pch_int_hmap_destroy(&intern_table);
}

// Get parse table -------------------------------------------------------------

bitset_t get_first_str(gt_node*);

bitset_t get_first_symb(gsymb_t sid)
// get first set of a symbol
{
    if(sid < 0 || sid >= NUM_GS)
    {
        fprintf(stderr, "get_first_nt: Invalid sid %d\n", sid);
        return empty_bitset;
    }
    else if(sid < (int)NUM_TOKENS)
        return bitset_singleton(sid);
    else
    {
        int ntid = get_nt_id(sid);
        if(first[ntid] == unset_bitset)
        {
            int i;
            first[ntid] = empty_bitset;
            for(i=rule_begin[ntid]; i < rule_end[ntid]; ++i)
                first[ntid] |= get_first_str(rules[i].head);
        }
        return first[ntid];
    }
}

bitset_t get_first_str(gt_node* node)
// get first set of a linked list string starting at node
{
    if(node == NULL)
        return bitset_singleton(T_EOF);

    if(node->first == unset_bitset)
    {
        int sid = node->value;
        bitset_t s = get_first_symb(sid);
        if(bitset_has(s, T_EOF))
            s = bitset_remove(s, T_EOF) | get_first_str(node->next);
        node->first = s;
    }
    return node->first;
}

void update_follow_helper(int rule_no)
// update follow using rule number i
{
    gt_node* n = rules[rule_no].head;
    for(; n != NULL; n = n->next)
    {
        gsymb_t sid = n->value;
        if(sid >= (int)NUM_TOKENS)
        {
            int ntid = get_nt_id(sid);
            bitset_t s = get_first_str(n->next);
            bitset_t new_follow = bitset_remove(s, T_EOF) | follow[ntid];
            if(follow[ntid] != new_follow)
            {
                follow_changed = true;
                follow[ntid] = new_follow;
            }
            if(bitset_has(s, T_EOF))
            {
                bitset_t s3 = follow[get_nt_id(rules[rule_no].lhs)];
                new_follow = s3 | follow[ntid];
                if(new_follow != follow[ntid])
                {
                    follow_changed = true;
                    follow[ntid] = new_follow;
                }
            }
        }
    }
}

void update_follow(gsymb_t start_symb)
{
    follow[get_nt_id(start_symb)] = bitset_singleton(T_EOF);
    follow_changed = true;
    while(follow_changed)
    {
        follow_changed = false;
        int i;
        for(i=0; i<num_rules; ++i)
            update_follow_helper(i);
    }
}

void print_bitset(bitset_t s, FILE* fp)
{
    if(s == unset_bitset)
        fprintf(fp, "{unset}\n");
    else
    {
        int i;
        fprintf(fp, "{");
        //fprintf(fp, "%llu {", s);
        bool first = true;
        for(i=0; i < NUM_TOKENS; ++i)
            if(bitset_has(s, i))
            {
                if(!first)
                    fprintf(fp, ", ");
                else
                    first = false;
                //fprintf(fp, "%s(%d)", GS_STRS[i], i);
                fprintf(fp, "%s", GS_STRS[i]);
            }
        fprintf(fp, "}\n");
    }
}

void print_firsts(FILE* fp)
{
    int i;
    for(i=0; i < NUM_NONTERMS; ++i)
    {
        fprintf(fp, "%s: ", GS_STRS[NUM_TOKENS + i]);
        print_bitset(get_first_symb(NUM_TOKENS + i), fp);
    }
}

void print_follows(FILE* fp)
{
    int i;
    for(i=0; i < NUM_NONTERMS; ++i)
    {
        fprintf(fp, "%s: ", GS_STRS[NUM_TOKENS + i]);
        print_bitset(follow[i], fp);
    }
}

void print_parse_table()
{
    for(int i=0; i<NUM_NONTERMS; i++)
        for(int j=0; j<NUM_TOKENS; j++)
            if(pt[i][j] != -1)
                printf("pt[%s][%s] = %d\n", GS_STRS[i+NUM_TOKENS], GS_STRS[j], pt[i][j]);
}

bool make_parse_table()
{
    int i;
    tok_t j;
    bool is_ll1 = true;
    for(i=0; i < num_rules; ++i)
    {
        int ntid = get_nt_id(rules[i].lhs);
        bitset_t fa = get_first_str(rules[i].head);
        bitset_t terms;
        if(bitset_has(fa, T_EOF))
            terms = follow[ntid] | bitset_remove(fa, T_EOF);
        else
            terms = fa;
        for(j=0; j < NUM_TOKENS; ++j)
            if(bitset_has(terms, j))
            {
                if(pt[ntid][j] != -1)
                    is_ll1 = false;
                pt[ntid][j] = i;
            }
    }
    return is_ll1;
}

void read_parse_table(const char* file_name){
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

parse_tree_node* handle_parse_error_1(const Token* ptok, int_stack* pst, parse_tree_node* tn)
// handle case where top of stack is a terminal and it doesn't match lookahead
{
    error_count++;
    gsymb_t cur_st_top = int_stack_top(pst);
    fprintf(stderr, "parse_error_1: line %2d, col %2d: \'%s\' (%s)\nGot %s (\'%s\') instead of %s.\n\n",
        ptok->line, ptok->col, ptok->lexeme, GS_STRS[ptok->tid],
        GS_STRS[ptok->tid], ptok->lexeme, GS_STRS[cur_st_top]);

    int_stack_pop(pst);
    return get_successor(tn);
}

parse_tree_node* handle_parse_error_2(const Token* ptok, int_stack* pst, parse_tree_node* tn)
// handle case where there is no entry in parse table
{
    error_count++;
    fprintf(stderr, "parse_error_2: line %2d, col %2d: \'%s\' (%s)\n%s (\'%s\') isn't expected here.\n\n",
        ptok->line, ptok->col, ptok->lexeme, GS_STRS[ptok->tid],
        GS_STRS[ptok->tid], ptok->lexeme);
    int_stack_pop(pst);
    return get_first_child_or_successor(tn);
}

parse_tree_node* build_parse_tree(FILE * ifp, gsymb_t start_sym){

    int_stack st;
    int_stack_init(&st);
    int_stack_push(&st, GS_EOF);
    int_stack_push(&st, start_sym);

    Token cur_tkn;
    Dfa dfa;
    init_dfa(&dfa);
    init_token(&cur_tkn);

    Symbol* symb = make_symbol(start_sym);
    parse_tree_node * current_tn = parse_tree_get_node(symb);
    parse_tree_node * root = current_tn;

    get_token(ifp, &dfa, &cur_tkn, false);
    //fprintf(stderr, "%s\n", GS_STRS[cur_tkn.tid]);
    do{
    gsymb_t cur_st_top = int_stack_top(&st);
    if(is_t(cur_st_top) || cur_st_top == GS_EOF){
        if(cur_st_top == cur_tkn.tid){
            int_stack_pop(&st);
            copy_symbol(current_tn->value, &cur_tkn);
            current_tn = get_successor(current_tn);
        }else{
            current_tn = handle_parse_error_1(&cur_tkn, &st, current_tn);
            //int_stack_print(&st, stderr);
        }
        get_token(ifp, &dfa, &cur_tkn, false);
        //fprintf(stderr, "%s\n", GS_STRS[cur_tkn.tid]);
        //int_stack_print(&st, stderr);
    }else{
        int rule_num = pt[get_nt_id(int_stack_top(&st))][cur_tkn.tid];
        //fprintf(stderr, "pt[%s, %d][%s, %d] = %d\n", GS_STRS[int_stack_top(&st)], get_nt_id(int_stack_top(&st)), GS_STRS[cur_tkn.tid], cur_tkn.tid, rule_num);
        if(rule_num != -1){
            int_stack_pop(&st);
            push_ll(&st, rules[rule_num].head);
            gt_node * tmp = rules[rule_num].head;
            current_tn->value->rule_num = rule_num;
            while(tmp != NULL){
                Symbol * ps = make_symbol(tmp->value);
                parse_tree_insert(current_tn, ps);
                tmp = tmp->next;
            }
            current_tn = get_first_child_or_successor(current_tn);
        }else{
            current_tn = handle_parse_error_2(&cur_tkn, &st, current_tn);
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
    error_count += dfa.error_count;
    return root;
}

// Output ----------------------------------------------------------------------

void print_node_sub(const parse_tree_node* root, FILE* fp)
{
    /*
    if(root == NULL)
        fprintf(fp, "print_node: NULL\n");
    else
        fprintf(fp, "print_node: %p %s\n", root, GS_STRS[root->value->tid]);
    */
    Symbol* s = root->value;
    char empty[] = "----";
    char* lexeme = (s->lexeme == NULL ? empty : s->lexeme);
    if((int)(s->sid) < NUM_TOKENS)
        fprintf(fp, "%20s %2d %20s ", lexeme, s->line, GS_STRS[s->sid]);
    else
        fprintf(fp, "%20s %2s %20s ", empty, "--", empty);

    if(s->sid == T_NUM)
        fprintf(fp, "%20d ", s->i);
    else if(s->sid == T_RNUM)
        fprintf(fp, "%20lf ", s->f);
    else
        fprintf(fp, "%20s ", empty);

    if(root->parent == NULL)
        fprintf(fp, "%20s ", empty);
    else
        fprintf(fp, "%20s ", GS_STRS[root->parent->value->sid]);

    if(root->first_child == NULL)
        fprintf(fp, "yes ");
    else
        fprintf(fp, " no ");

    fprintf(fp, "%20s\n", GS_STRS[s->sid]);
}

void print_tree_sub(const parse_tree_node* root, FILE* fp)
{
    if(root != NULL)
    {
        parse_tree_node* n = root->first_child;
        if(n != NULL)
            print_tree_sub(n, fp);
        print_node_sub(root, fp);
        if(n != NULL)
            for(n = n->next_sibling; n != NULL; n = n->next_sibling)
                print_tree_sub(n, fp);
    }
}

void print_node(const parse_tree_node* root, FILE* fp)
{
    if(root != NULL)
    {
        fprintf(fp, "%s  ", GS_STRS[root->value->sid]);
        if(root->value->rule_num >= 0)
            fprintf(fp, "(%2d)  ", root->value->rule_num);
        if(root->value->lexeme != NULL)
            fprintf(fp, "%s", root->value->lexeme);
        else if(root->value->sid == T_NUM)
            fprintf(fp, "%d", root->value->i);
        else if(root->value->sid == T_RNUM)
            fprintf(fp, "%lf", root->value->f);
        fprintf(fp, "\n");
    }
}

void print_tree_helper(const parse_tree_node* root, FILE* fp, int indent)
{
    if(root != NULL)
    {
        for(int i=0; i<indent; ++i)
            fprintf(fp, "  ");
        print_node(root, fp);
        parse_tree_node* p = root->first_child;
        for(; p != NULL; p = p->next_sibling)
            print_tree_helper(p, fp, indent + 1);
    }
}

void print_tree(const parse_tree_node* root, FILE* fp)
{print_tree_helper(root, fp, 0);}

void dont_print_tree(const parse_tree_node* root, FILE* fp){}

int parser_main(FILE* ifp, FILE* ofp, int verbosity, parse_tree_printer tp)
{
    gsymb_t start_symb = init_parser();

    if(verbosity > 0)
    {
        print_firsts(stdout);
        printf("\n");
        print_follows(stdout);
        print_parse_table();
    }

    parse_tree_node* root = build_parse_tree(ifp, start_symb);
    tp(root, ofp);
    parse_tree_destroy(root);

    destroy_parser();
    return error_count;
}

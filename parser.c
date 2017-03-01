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

static pch_int_hmap gsymb_ht;

char* GS_STRS[] = {
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

void init_symbol(Symbol * symb){
    symb->line = 0;
    symb->col = 0;
    symb->lexeme = NULL;
    symb->size = 0;
    symb->tid = T_ERR;
    symb->num.f = 0.0;
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

int get_nt_id(gsymb_t s){ return s - NUM_TOKENS; } 

int pt[NUM_NONTERMS][NUM_TOKENS];

void read_parse_table(const char file_name []){
    for(int i=0; i<NUM_NONTERMS; i++)
	for(int j=0; j<NUM_TOKENS; j++)
	    pt[i][j] = -1;
    FILE * fp = fopen(file_name, "r");
    if(fp == NULL){
		fprintf(stderr,"File %s not found\n",file_name);
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
}

//read_parse_table
//global parse_table
//void read_parse_table(FILE * fp)

//build_parse_tree
//tree_t build_parse_tree(Rule * rules, parse_table);

bool is_t(gsymb_t x)
{ return (int)x < NUM_TOKENS; }
bool is_nt(gsymb_t x)
{ return (int)x >= NUM_TOKENS && (int)x < NUM_GS; }

void push_ll(int_Stack* pst, gt_node * head){
    if(head == NULL) return;
    push_ll(pst,head->next);
    int_stack_push(pst, head->value);
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
    //fprintf(stderr,"%s\n",GS_STRS[cur_tkn.tid]);
    do{
	gsymb_t cur_st_top = int_stack_top(&st);
	if(is_t(cur_st_top) || cur_st_top == GS_EOF){
	    if(cur_st_top == cur_tkn.tid){
			int_stack_pop(&st);
            copy_symbol(current_tn->symbol, &cur_tkn);
			current_tn = get_successor(current_tn);
			get_token(ifp, &dfa, &cur_tkn, false);
			//fprintf(stderr,"%s\n",GS_STRS[cur_tkn.tid]);
            //int_stack_print(&st,stderr);
	    }else{
			fprintf(stderr,"No valid derivation of string exists! 1\n");
			fprintf(stderr,"Current stack top: %s\n",GS_STRS[cur_st_top]);
			fprintf(stderr,"Current Token: %s\n",GS_STRS[cur_tkn.tid]);
            //int_stack_print(&st,stderr);
	    }
	}else{
	    int rule_num = pt[get_nt_id(int_stack_top(&st))][cur_tkn.tid];
	    //fprintf(stderr, "pt[%s,%d][%s,%d] = %d\n", GS_STRS[int_stack_top(&st)], get_nt_id(int_stack_top(&st)), GS_STRS[cur_tkn.tid], cur_tkn.tid, rule_num);
	    if(rule_num != -1){
			int_stack_pop(&st);
			push_ll(&st, rules[rule_num].head);
			gt_node * tmp = rules[rule_num].head;
			while(tmp != NULL){
				Symbol * ps = make_symbol(tmp->value);
				insert_node(current_tn,ps);
				tmp = tmp->next;
			}
			if(current_tn->first_child != NULL) current_tn = current_tn->first_child;
			else current_tn = get_successor(current_tn);
		}else{
			fprintf(stderr,"No valid derivation of string exists! 2\n");
			fprintf(stderr,"Current Token: %s\n",GS_STRS[cur_tkn.tid]);
            //int_stack_print(&st,stderr);
	    }
	}
    }while(st.size > 1);
    if(int_stack_top(&st) != GS_EOF){
		fprintf(stderr, "No valid derivation of string exists! 3\n");
		fprintf(stderr, "Current Token: %s\n",GS_STRS[cur_tkn.tid]);
        //int_stack_print(&st,stderr);
    }
    return root;
}

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
    return start_symb;
}

void print_node(TreeNode* root, FILE* fp)
{
    /*
    if(root == NULL)
        fprintf(fp, "print_node: NULL\n");
    else
        fprintf(fp, "print_node: %p %s\n", root, GS_STRS[root->symbol->tid]);
    */
    Symbol* s = root->symbol;
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
        fprintf(fp, "%20s ", GS_STRS[root->parent->symbol->tid]);

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
	char pt_fname[] = "data/parse_table.txt";
    read_parse_table(pt_fname);
    
    /*
    for(int i=0; i<NUM_NONTERMS; i++){
	for(int j=0; j<NUM_TOKENS; j++)
	    printf("%d ", pt[i][j]);
	printf("\n");
    }
    */
    
    TreeNode* root = build_parse_tree(ifp, start_symb);
    print_tree(root, ofp);

    return 0;
}

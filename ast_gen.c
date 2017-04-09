#include <stdio.h>
#include "parser.h"
#include "ast.h"
#include "symbol_defs.h"

int read_children(parse_tree_node** nodes, parse_tree_node* root) {
    //for(i=0; i<n; ++i) nodes[i] = NULL;
    if(root == NULL) return -1;
    root = root->first_child;
    int n;
    for(n=0; root != NULL; ++n) {
        nodes[n] = root;
        root = root->next_sibling;
    }
    nodes[n] = NULL;
    return n;
}

void build_ast(parse_tree_node* p) {
    if(p == NULL) return;
    parse_tree_node* n[20];
    Symbol* s[20];
    n[0] = p;
    int i, num = read_children(n+1, p);
    for(i=0; i<=num; ++i)
        s[i] = n[i]->value;
    //fprintf(stderr, "build_ast called on %s (rule %d)\n", GS_STRS[p->value->sid], p->value->rule_num);
    switch(p->value->rule_num) {
        case 0:
            s[4]->driver = NULL;
            build_ast(n[4]);
            s[3]->next = s[4]->tree;
            build_ast(n[3]);
            s[2]->driver = s[3]->tree;
            build_ast(n[2]);
            build_ast(n[1]);
            ProgramNode* program_node = (ProgramNode*)get_ast_node(ASTN_Program);
            program_node->decls = s[1]->tree;
            program_node->driver = s[3]->tree;
            program_node->modules = s[2]->tree;
            s[0]->tree = program_node;
            break;
        case 1:
        case 9:
            build_ast(n[2]);
            s[1]->next = s[2]->tree;
            build_ast(n[1]);
            s[0]->tree = s[1]->tree;
            break;
        case 2:
            s[0]->tree = NULL;
            break;
        case 3: {
            IDListNode* mynode = (IDListNode*)get_ast_node(ASTN_IDList);
            mynode->varname = s[3]->lexeme;
            s[0]->tree = mynode;
            break;
        }
        case 4:
            s[2]->driver = s[0]->driver;
            build_ast(n[2]);
            s[1]->next = s[2]->tree;
            build_ast(n[1]);
            s[0]->tree = s[1]->tree;
            break;
        case 5:
            s[0]->tree = s[0]->driver;
            break;
        case 6: {
            build_ast(n[5]);
            ModuleNode* mynode = (ModuleNode*)get_ast_node(ASTN_Module);
            mynode->name = NULL;
            mynode->iParamList = NULL;
            mynode->oParamList = NULL;
            mynode->body = s[5]->tree;
            mynode->next = s[0]->next;
            s[0]->tree = mynode;
            break;
        }
        case 7: {
            build_ast(n[8]);
            build_ast(n[11]);
            build_ast(n[12]);
            ModuleNode* mynode = (ModuleNode*)get_ast_node(ASTN_Module);
            mynode->name = s[3]->lexeme;
            mynode->iParamList = s[8]->tree;
            mynode->oParamList = s[11]->tree;
            mynode->body = s[12]->tree;
            mynode->next = s[0]->next;
            s[0]->tree = mynode;
            break;
        }
        case 8:
            build_ast(n[2]);
            s[0]->tree = s[2]->tree;
            break;
        case 10:
            s[0]->tree = NULL;
            break;
        default:
            s[0]->tree = NULL;
    }
}

int ast_gen_main(FILE* ifp, FILE* ofp, int verbosity)
{
    gsymb_t start_symb = init_parser();

    parse_tree_node* proot = build_parse_tree(ifp, start_symb);
    build_ast(proot);
    pAstNode ast = proot->value->tree;
    parse_tree_destroy(proot);
    destroy_parser(false);

    print_ast(stdout, ast, 0);

    pch_int_hmap_destroy(&intern_table);
    return parser_error_count;
}

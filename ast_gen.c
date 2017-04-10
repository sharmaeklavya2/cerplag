#include <stdio.h>
#include "parser.h"
#include "ast.h"
#include "symbol_defs.h"
#include "error.h"

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
        case 13:
        case 17:
        case 60:
            build_ast(n[2]);
            s[1]->next = s[2]->tree;
            build_ast(n[1]);
            s[0]->tree = s[1]->tree;
            break;
        case 2:
        case 10:
        case 12:
        case 15:
        case 19:
        case 54:
        case 57:
        case 61:
        case 64:
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
        case 53:
        case 85:
            build_ast(n[2]);
            s[0]->tree = s[2]->tree;
            break;
        case 11:
            build_ast(n[3]);
            s[0]->tree = s[3]->tree;
            break;
        case 14:
        case 18:
            build_ast(n[3]);
            s[2]->next = s[3]->tree;
            build_ast(n[2]);
            s[0]->tree = s[2]->tree;
            break;
        case 16: {
            build_ast(n[3]);
            IDTypeListNode* mynode = (IDTypeListNode*)get_ast_node(ASTN_IDTypeList);
            mynode->varname = s[1]->lexeme;
            mynode->base.type = s[3]->type;
            mynode->base.size = 0;
            mynode->next = s[0]->next;
            s[0]->tree = mynode;
            break;
        }
        case 20: {
            build_ast(n[3]);
            IDTypeListNode* mynode = (IDTypeListNode*)get_ast_node(ASTN_IDTypeList);
            mynode->varname = s[1]->lexeme;
            mynode->base.type = s[3]->type;
            mynode->base.size = s[3]->size;
            mynode->next = s[0]->next;
            s[0]->tree = mynode;
            break;
        }

        case 21:
            s[0]->type = TYPE_INTEGER;
            break;
        case 22:
            s[0]->type = TYPE_REAL;
            break;
        case 23:
            s[0]->type = TYPE_BOOLEAN;
            break;
        case 24:
            build_ast(n[1]);
            s[0]->type = s[1]->type;
            s[0]->size = 0;
            break;
        case 25:
            build_ast(n[3]);
            build_ast(n[6]);
            s[0]->type = s[6]->type;
            s[0]->size = s[3]->end;
            if(s[3]->beg != 1) {
                // TODO: handle error correctly
                print_error("ast_gen", 1, -1, -1, NULL, NULL, "Start index of array should be 1.");
            }
            break;
        case 26:
            s[2]->varname = s[1]->lexeme;
            build_ast(n[2]);
            s[0]->tree = s[2]->tree;
            break;
        case 27: {
            build_ast(n[2]);
            DerefNode* mynode = (DerefNode*)get_ast_node(ASTN_Deref);
            mynode->varname = s[0]->varname;
            mynode->index = s[2]->tree;
            s[0]->tree = mynode;
            break;
        }
        case 28: {
            VarNode* mynode = (VarNode*)get_ast_node(ASTN_Var);
            mynode->varname = s[0]->varname;
            s[0]->tree = mynode;
            break;
        }
        case 29:
        case 36:
        case 37:
        case 83:
        case 84:
            build_ast(n[1]);
            s[0]->tree = s[1]->tree;
            break;
        case 30:
        case 32:
        case 38: {
            NumNode* mynode = (NumNode*)get_ast_node(ASTN_Num);
            mynode->val = s[1]->i;
            s[0]->tree = mynode;
            break;
        }
        case 31:
        case 33: {
            RNumNode* mynode = (RNumNode*)get_ast_node(ASTN_RNum);
            mynode->val = s[1]->f;
            s[0]->tree = mynode;
            break;
        }
        case 34: {
            BoolNode* mynode = (BoolNode*)get_ast_node(ASTN_Bool);
            mynode->val = true;
            s[0]->tree = mynode;
            break;
        }
        case 35: {
            BoolNode* mynode = (BoolNode*)get_ast_node(ASTN_Bool);
            mynode->val = false;
            s[0]->tree = mynode;
            break;
        }
        case 39: {
            VarNode* mynode = (VarNode*)get_ast_node(ASTN_Var);
            mynode->varname = s[1]->lexeme;
            s[0]->tree = mynode;
            break;
        }
        case 40:
            s[0]->beg = s[1]->i;
            s[0]->end = s[3]->i;
            break;
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
        case 48:
            s[1]->next = s[0]->next;
            build_ast(n[1]);
            s[0]->tree = s[1]->tree;
            //print_ast(stdout,s[0]->tree,0);
            break;
        case 49:;
            InputNode* input_node = (InputNode*)get_ast_node(ASTN_Input);
            input_node->next = s[0]->next;
            input_node->varname = s[3]->lexeme;
            s[0]->tree = input_node;
            //print_ast(stdout,s[0]->tree,0);
            break;
        case 50:;
            OutputNode* output_node = (OutputNode*)get_ast_node(ASTN_Output);
            output_node->next = s[0]->next;
            build_ast(n[3]);
            output_node->var = s[3]->tree;
            s[0]->tree = output_node;
            //print_ast(stdout,s[1]->tree,0);
            break;
        case 51:;
            AssnNode* assn_node = (AssnNode*)get_ast_node(ASTN_Assn);
            build_ast(n[1]);
            build_ast(n[3]);
            assn_node->target = s[1]->tree;
            assn_node->expr = s[3]->tree;
            assn_node->next = s[0]->next;
            s[0]->tree = assn_node;
            break;
        case 52:
            build_ast(n[7]);
            build_ast(n[1]);
            FCallNode* fcall_node = (FCallNode*)get_ast_node(ASTN_FCall);
            fcall_node->name = s[4]->lexeme;
            fcall_node->iParamList = s[7]->tree;
            fcall_node->oParamList = s[1]->tree;
            fcall_node->next = s[0]->next;
            s[0]->tree = fcall_node;
            break;
        case 55: {
            build_ast(n[2]);
            IDListNode* idlist_node = (IDListNode*)get_ast_node(ASTN_IDList);
            idlist_node->next = s[2]->tree;
            idlist_node->varname = s[1]->lexeme;
            s[0]->tree = idlist_node;
            break;
        }
        case 56: {
            build_ast(n[3]);
            IDListNode* idlist_node = (IDListNode*)get_ast_node(ASTN_IDList);
            idlist_node->varname = s[2]->lexeme;
            idlist_node->next = s[3]->tree;
            s[0]->tree = idlist_node;
            break;
        }
        case 58: {
            build_ast(n[2]);
            build_ast(n[4]);
            DeclNode* decl_node = (DeclNode*)get_ast_node(ASTN_Decl);
            decl_node->next = s[0]->next;
            decl_node->idList = s[2]->tree;
            decl_node->base.type = s[4]->type;
            decl_node->base.size = s[4]->size;
            s[0]->tree = decl_node;
            break;
        }
        case 59: {
            build_ast(n[7]);
            s[6]->next = s[7]->tree;
            build_ast(n[6]);
            build_ast(n[8]);
            SwitchNode* switch_node = (SwitchNode*)get_ast_node(ASTN_Switch);
            switch_node->varname = s[3]->lexeme;
            switch_node->next = s[0]->next;
            switch_node->defaultcase = s[8]->tree;
            switch_node->cases = s[6]->tree;
            s[0]->tree = switch_node;
            break;
        }
        case 62: {
            CaseNode* case_node = (CaseNode*)get_ast_node(ASTN_Case);
            build_ast(n[2]);
            build_ast(n[4]);
            case_node->val = s[2]->tree;
            case_node->stmts = s[4]->tree;
            case_node->next = s[0]->next;
            s[0]->tree = case_node;
            break;
        }
        case 63: {
            CaseNode* case_node = (CaseNode*)get_ast_node(ASTN_Case);
            build_ast(n[3]);
            case_node->val = NULL;
            case_node->next = NULL;
            s[0]->tree = case_node;
            break;
        }
        case 65: {
            build_ast(n[8]);
            ForNode* for_node = (ForNode*)get_ast_node(ASTN_For);
            for_node->beg = s[5]->beg;
            for_node->end = s[5]->end;
            for_node->varname = s[3]->lexeme;
            for_node->body = s[8]->tree;
            for_node->next = s[0]->next;
            s[0]->tree = for_node;
            break;
        }
        case 66: {
            build_ast(n[6]);
            build_ast(n[3]);
            WhileNode* while_node = (WhileNode*)get_ast_node(ASTN_While);
            while_node->cond = s[3]->tree;
            while_node->body = s[6]->tree;
            while_node->next = s[0]->next;
            s[0]->tree = while_node;
            break;
        }
        case 67:
        case 70:
        case 73:
        case 76:
        case 79:
            build_ast(n[1]);
            s[2]->acc = s[1]->tree;
            build_ast(n[2]);
            s[0]->tree = s[2]->tree;
            break;
        case 69:
        case 72:
        case 75:
        case 78:
        case 81:
            s[0]->tree = s[0]->acc;
            break;
        case 68: {
            build_ast(n[2]);
            BOpNode* mynode = (BOpNode*)get_ast_node(ASTN_BOp);
            mynode->op = OP_OR;
            mynode->arg1 = s[0]->acc;
            mynode->arg2 = s[2]->tree;
            s[0]->tree = mynode;
            break;
        }
        case 71: {
            build_ast(n[2]);
            BOpNode* mynode = (BOpNode*)get_ast_node(ASTN_BOp);
            mynode->op = OP_AND;
            mynode->arg1 = s[0]->acc;
            mynode->arg2 = s[2]->tree;
            s[0]->tree = mynode;
            break;
        }
        case 74:
        case 77:
        case 80: {
            build_ast(n[1]);
            build_ast(n[2]);
            BOpNode* mynode = (BOpNode*)get_ast_node(ASTN_BOp);
            mynode->op = s[1]->op;
            mynode->arg1 = s[0]->acc;
            mynode->arg2 = s[2]->tree;
            s[0]->tree = mynode;
            break;
        }
        case 82: {
            build_ast(n[1]);
            build_ast(n[2]);
            UOpNode* mynode = (UOpNode*)get_ast_node(ASTN_UOp);
            mynode->op = s[1]->op;
            mynode->arg = s[2]->tree;
            s[0]->tree = mynode;
            break;
        }
        case 86: s[0]->op = OP_PLUS; break;
        case 87: s[0]->op = OP_MINUS; break;
        case 88: s[0]->op = OP_LT; break;
        case 89: s[0]->op = OP_LE; break;
        case 90: s[0]->op = OP_GT; break;
        case 91: s[0]->op = OP_GE; break;
        case 92: s[0]->op = OP_EQ; break;
        case 93: s[0]->op = OP_NE; break;
        case 94: s[0]->op = OP_MUL; break;
        case 95: s[0]->op = OP_DIV; break;
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
    return 0;
}

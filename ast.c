#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast_defs.h"

char* ASTN_STRS[] = {
#define X(a, b) #a"Node",
#include "data/ast_nodes.xmac"
#undef X
};

char* TYPE_STRS[] = TYPE_STRS_LIST;
char* OP_STRS[] = OP_STRS_LIST;
int TYPE_SIZES[] = TYPE_SIZES_LIST;
int TYPE_ALIGNS[] = TYPE_SIZES_LIST;

void complain_ast_node_type(const char* funcname, astn_t node_type) {
    if(node_type >= NUM_ASTN)
        fprintf(stderr, "%s: invalid node_type %d\n", funcname, node_type);
    else
        fprintf(stderr, "%s: invalid node_type %s (%d)\n", funcname, ASTN_STRS[node_type], node_type);
}

pAstNode get_ast_node(astn_t node_type) {
    pAstNode p = NULL;
    switch(node_type) {
#define X(a, b) case ASTN_##a: p = malloc(sizeof(a##Node)); memset(p, 0, sizeof(a##Node)); p->base.node_type = node_type; break;
#include "data/ast_nodes.xmac"
#undef X
        default:
            complain_ast_node_type(__func__, node_type);
    }
    return p;
}

#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"

pAstNode get_next_ast_node(pAstNode p) {
    switch(p->base.node_type) {
        case ASTN_IDList: return ((IDListNode*)p)->next;
        case ASTN_IDTypeList: return ((IDTypeListNode*)p)->next;
        case ASTN_Module: return ((ModuleNode*)p)->next;
        case ASTN_Assn: return ((AssnNode*)p)->next;
        case ASTN_While: return ((WhileNode*)p)->next;
        case ASTN_For: return ((ForNode*)p)->next;
        case ASTN_Decl: return ((DeclNode*)p)->next;
        case ASTN_Input: return ((InputNode*)p)->next;
        case ASTN_Output: return ((OutputNode*)p)->next;
        case ASTN_FCall: return ((FCallNode*)p)->next;
        case ASTN_Switch: return ((SwitchNode*)p)->next;
        case ASTN_Case: return ((CaseNode*)p)->next;
        default:
            complain_ast_node_type(__func__, p->base.node_type);
            return NULL;
    }
}

static void print_type(FILE* fp, valtype_t type, int size) {
    fprintf(fp, "%s", TYPE_STRS[type]);
    if(size > 0)
        fprintf(fp, "[%d]", size);
}

void print_ast(FILE* fp, pAstNode p, int indent) {
    int i;
    const char INDENT_STR[] = "  ";
    const char* INITIAL_INDENT = "        ";
    if(p != NULL)
        fprintf(fp, "(%2d:%2d) ", p->base.line, p->base.col);
    else
        fprintf(fp, "%s", INITIAL_INDENT);
    for(i=0; i<indent; ++i)
        fprintf(fp, INDENT_STR);
    if(p == NULL) {
        fprintf(fp, "NULL\n");
        return;
    }
    fprintf(fp, "%s", ASTN_STRS[p->base.node_type]);
    switch(p->base.node_type) {
        case ASTN_BOp:
            fprintf(fp, "(%s):\n", OP_STRS[((BOpNode*)p)->op]);
            print_ast(fp, ((BOpNode*)p)->arg1, indent+1);
            print_ast(fp, ((BOpNode*)p)->arg2, indent+1);
            break;
        case ASTN_UOp:
            fprintf(fp, "(%s):\n", OP_STRS[((UOpNode*)p)->op]);
            print_ast(fp, ((UOpNode*)p)->arg, indent+1);
            break;
        case ASTN_Deref:
            fprintf(fp, "(%s):\n", ((DerefNode*)p)->varname);
            print_ast(fp, ((DerefNode*)p)->index, indent+1);
            break;

        case ASTN_Var:
            fprintf(fp, "(%s)\n", ((VarNode*)p)->varname);
            break;
        case ASTN_Num:
            fprintf(fp, "(%d)\n", ((NumNode*)p)->val);
            break;
        case ASTN_RNum:
            fprintf(fp, "(%lf)\n", ((RNumNode*)p)->val);
            break;
        case ASTN_Bool:
            fprintf(fp, "(%s)\n", (((BoolNode*)p)->val)?"true":"false");
            break;
        case ASTN_Range:
            fprintf(fp, "(%d, %d)\n", ((RangeNode*)p)->beg, ((RangeNode*)p)->end);
            break;

        case ASTN_IDList:
            fprintf(fp, "(%s)\n", ((IDListNode*)p)->varname);
            if(((IDListNode*)p)->next != NULL)
                print_ast(fp, ((IDListNode*)p)->next, indent);
            break;
        case ASTN_IDTypeList:
            fprintf(fp, "(%s, ", ((IDTypeListNode*)p)->varname);
            print_type(fp, ((IDTypeListNode*)p)->base.type, ((IDTypeListNode*)p)->base.size);
            fprintf(fp, ")\n");
            if(((IDTypeListNode*)p)->next != NULL)
                print_ast(fp, ((IDTypeListNode*)p)->next, indent);
            break;

        case ASTN_Module:
            fprintf(fp, "(%s):\n", ((ModuleNode*)p)->name);

            for(i=-1; i<indent; ++i) fprintf(fp, INDENT_STR);
            fprintf(fp, "%siParamList=\n", INITIAL_INDENT);
            print_ast(fp, ((ModuleNode*)p)->iParamList, indent+2);

            for(i=-1; i<indent; ++i) fprintf(fp, INDENT_STR);
            fprintf(fp, "%soParamList=\n", INITIAL_INDENT);
            print_ast(fp, ((ModuleNode*)p)->oParamList, indent+2);

            for(i=-1; i<indent; ++i) fprintf(fp, INDENT_STR);
            fprintf(fp, "%sbody=\n", INITIAL_INDENT);
            print_ast(fp, ((ModuleNode*)p)->body, indent+2);

            if(((ModuleNode*)p)->next != NULL)
                print_ast(fp, ((ModuleNode*)p)->next, indent);
            break;
        case ASTN_Program:
            fprintf(fp, ":\n");
            print_ast(fp, ((ProgramNode*)p)->decls, indent+1);
            print_ast(fp, ((ProgramNode*)p)->modules, indent+1);
            break;

        case ASTN_Assn:
            fprintf(fp, ":\n");

            for(i=-1; i<indent; ++i) fprintf(fp, INDENT_STR);
            fprintf(fp, "%starget=\n", INITIAL_INDENT);
            print_ast(fp, ((AssnNode*)p)->target, indent+2);

            for(i=-1; i<indent; ++i) fprintf(fp, INDENT_STR);
            fprintf(fp, "%sexpr=\n", INITIAL_INDENT);
            print_ast(fp, ((AssnNode*)p)->expr, indent+2);

            if(((AssnNode*)p)->next != NULL)
                print_ast(fp, ((AssnNode*)p)->next, indent);
            break;

        case ASTN_While:
            fprintf(fp, ":\n");

            for(i=-1; i<indent; ++i) fprintf(fp, INDENT_STR);
            fprintf(fp, "%scond=\n", INITIAL_INDENT);
            print_ast(fp, ((WhileNode*)p)->cond, indent+2);

            for(i=-1; i<indent; ++i) fprintf(fp, INDENT_STR);
            fprintf(fp, "%sbody=\n", INITIAL_INDENT);
            print_ast(fp, ((WhileNode*)p)->body, indent+2);

            if(((WhileNode*)p)->next != NULL)
                print_ast(fp, ((WhileNode*)p)->next, indent);
            break;

        case ASTN_For:
            fprintf(fp, "(%s, %d, %d):\n", ((ForNode*)p)->varname, ((ForNode*)p)->beg, ((ForNode*)p)->end);
            print_ast(fp, ((ForNode*)p)->body, indent+1);
            if(((ForNode*)p)->next != NULL)
                print_ast(fp, ((ForNode*)p)->next, indent);
            break;

        case ASTN_Decl:
            fprintf(fp, "(");
            print_type(fp, ((DeclNode*)p)->base.type, ((DeclNode*)p)->base.size);
            fprintf(fp, "):\n");
            print_ast(fp, ((DeclNode*)p)->idList, indent+1);
            if(((DeclNode*)p)->next != NULL)
                print_ast(fp, ((DeclNode*)p)->next, indent);
            break;

        case ASTN_Input:
            fprintf(fp, "(%s)\n", ((InputNode*)p)->varname);
            if(((InputNode*)p)->next != NULL)
                print_ast(fp, ((InputNode*)p)->next, indent);
            break;

        case ASTN_Output:
            fprintf(fp, ":\n");
            print_ast(fp, ((OutputNode*)p)->var, indent+1);
            if(((OutputNode*)p)->next != NULL)
                print_ast(fp, ((OutputNode*)p)->next, indent);
            break;

        case ASTN_FCall:
            fprintf(fp, "(%s):\n", ((FCallNode*)p)->name);

            for(i=-1; i<indent; ++i) fprintf(fp, INDENT_STR);
            fprintf(fp, "%siParamList=\n", INITIAL_INDENT);
            print_ast(fp, ((FCallNode*)p)->iParamList, indent+2);

            for(i=-1; i<indent; ++i) fprintf(fp, INDENT_STR);
            fprintf(fp, "%soParamList=\n", INITIAL_INDENT);
            print_ast(fp, ((FCallNode*)p)->oParamList, indent+2);

            if(((FCallNode*)p)->next != NULL)
                print_ast(fp, ((FCallNode*)p)->next, indent);
            break;

        case ASTN_Switch:
            fprintf(fp, "(%s):\n", ((SwitchNode*)p)->varname);

            for(i=-1; i<indent; ++i) fprintf(fp, INDENT_STR);
            fprintf(fp, "%sdefaultcase=\n", INITIAL_INDENT);
            print_ast(fp, ((SwitchNode*)p)->defaultcase, indent+2);

            for(i=-1; i<indent; ++i) fprintf(fp, INDENT_STR);
            fprintf(fp, "%scases=\n", INITIAL_INDENT);
            print_ast(fp, ((SwitchNode*)p)->cases, indent+2);

            if(((SwitchNode*)p)->next != NULL)
                print_ast(fp, ((SwitchNode*)p)->next, indent);
            break;

        case ASTN_Case:
            fprintf(fp, ":\n");

            for(i=-1; i<indent; ++i) fprintf(fp, INDENT_STR);
            fprintf(fp, "%sval=\n", INITIAL_INDENT);
            print_ast(fp, ((CaseNode*)p)->val, indent+2);

            for(i=-1; i<indent; ++i) fprintf(fp, INDENT_STR);
            fprintf(fp, "%sstmts=\n", INITIAL_INDENT);
            print_ast(fp, ((CaseNode*)p)->stmts, indent+2);

            if(((CaseNode*)p)->next != NULL)
                print_ast(fp, ((CaseNode*)p)->next, indent);
            break;
        default:
            complain_ast_node_type(__func__, p->base.node_type);
    }
}

void destroy_ast(pAstNode p) {
    if(p == NULL) return;
    switch(p->base.node_type) {
        case ASTN_BOp:
            destroy_ast(((BOpNode*)p)->arg1);
            destroy_ast(((BOpNode*)p)->arg2);
            break;
        case ASTN_UOp:
            destroy_ast(((UOpNode*)p)->arg);
            break;
        case ASTN_Deref:
            destroy_ast(((DerefNode*)p)->index);
            break;
        case ASTN_IDList:
            destroy_ast(((IDListNode*)p)->next);
            break;
        case ASTN_IDTypeList:
            destroy_ast(((IDTypeListNode*)p)->next);
            break;
        case ASTN_Module:
            destroy_ast(((ModuleNode*)p)->next);
            destroy_ast(((ModuleNode*)p)->iParamList);
            destroy_ast(((ModuleNode*)p)->oParamList);
            destroy_ast(((ModuleNode*)p)->body);
            break;
        case ASTN_Program:
            destroy_ast(((ProgramNode*)p)->decls);
            destroy_ast(((ProgramNode*)p)->modules);
            break;
        case ASTN_Assn:
            destroy_ast(((AssnNode*)p)->next);
            destroy_ast(((AssnNode*)p)->target);
            destroy_ast(((AssnNode*)p)->expr);
            break;
        case ASTN_While:
            destroy_ast(((WhileNode*)p)->next);
            destroy_ast(((WhileNode*)p)->cond);
            destroy_ast(((WhileNode*)p)->body);
            break;
        case ASTN_For:
            destroy_ast(((ForNode*)p)->next);
            destroy_ast(((ForNode*)p)->body);
            break;
        case ASTN_Decl:
            destroy_ast(((DeclNode*)p)->next);
            destroy_ast(((DeclNode*)p)->idList);
            break;
        case ASTN_Input:
            destroy_ast(((InputNode*)p)->next);
            break;
        case ASTN_Output:
            destroy_ast(((OutputNode*)p)->next);
            destroy_ast(((OutputNode*)p)->var);
            break;
        case ASTN_FCall:
            destroy_ast(((FCallNode*)p)->next);
            destroy_ast(((FCallNode*)p)->iParamList);
            destroy_ast(((FCallNode*)p)->oParamList);
            break;
        case ASTN_Switch:
            destroy_ast(((SwitchNode*)p)->next);
            destroy_ast(((SwitchNode*)p)->cases);
            destroy_ast(((SwitchNode*)p)->defaultcase);
            break;
        case ASTN_Case:
            destroy_ast(((CaseNode*)p)->next);
            destroy_ast(((CaseNode*)p)->val);
            destroy_ast(((CaseNode*)p)->stmts);
            break;
        default:;
    }
    free(p);
}

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "parser.h"
#include "error.h"
#include "ast.h"
#include "ast_gen.h"
#include "symbol_table.h"
#include "util/vptr_int_hmap.h"

#define MODULE_DECLARED 1
#define MODULE_DEFINED  2
#define MODULE_USED     4

typedef ModuleNode* pMN;

static void pMN_destroy(pMN p){
}
static void pMN_print(pMN p, FILE* fp)
{fprintf(fp, "ModuleNode(%s, %d:%d)", p->name, p->base.line, p->base.col);}

#define KTYPE vptr
#define VTYPE pMN
#define KTYPED(x) vptr_##x
#define VTYPED(x) pMN_##x
#define ITYPED(x) vptr_pMN_##x
#include "util/hmap.gen.h"
#include "util/hmap.gen.c"
#undef KTYPE
#undef VTYPE
#undef KTYPED
#undef VTYPED
#undef ITYPED

vptr_int_hmap module_status;
vptr_pMN_hmap module_node_map;
STStack mySTStack;

static char msg[200];

void print_type_error(op_t op, valtype_t type1, valtype_t type2, int line, int col) {
    sprintf(msg, "Operands are of the wrong type ('%s' and '%s') for operator '%s'.",
        TYPE_STRS[type1], TYPE_STRS[type2], OP_STRS[op]);
    print_error("type", ERROR, 10, line, col, NULL, NULL, msg);
}

void print_undecl_id_error(const char* varname, int line, int col) {
    print_error("compile", ERROR, 11, line, col, varname, "UNDECL_ID", "Undeclared identifier");
}

valtype_t get_composite_type(op_t op, valtype_t type1, valtype_t type2, int line, int col) {
    if(type1 == TYPE_ERROR || type2 == TYPE_ERROR)
        return TYPE_ERROR;
    if(type1 != type2) {
        print_type_error(op, type1, type2, line, col);
        return TYPE_ERROR;
    }
    switch(op) {
        case OP_PLUS:
        case OP_MINUS:
        case OP_MUL:
            if(type1 == TYPE_INTEGER || type1 == TYPE_REAL)
                return type1;
            else
                print_type_error(op, type1, type2, line, col);
            break;
        case OP_DIV:
            if(type1 == TYPE_INTEGER || type1 == TYPE_REAL)
                return TYPE_REAL;
            else
                print_type_error(op, type1, type2, line, col);
            break;
        case OP_GT:
        case OP_LT:
        case OP_GE:
        case OP_LE:
        case OP_EQ:
        case OP_NE:
            if(type1 == TYPE_INTEGER || type1 == TYPE_REAL)
                return TYPE_BOOLEAN;
            else
                print_type_error(op, type1, type2, line, col);
            break;
        case OP_AND:
        case OP_OR:
            if(type1 == TYPE_BOOLEAN)
                return TYPE_BOOLEAN;
            else
                print_type_error(op, type1, type2, line, col);
            break;
        default:
            fprintf(stderr, "Invalid operand %d\n", op);
    }
    return TYPE_ERROR;
}

void compile_node(pAstNode p);

void compile_node_chain(pAstNode p) {
    while(p != NULL) {
        compile_node(p);
        p = get_next_ast_node(p);
    }
}

static void get_type_str(char* str, valtype_t type, int size) {
    if(size == 0)
        sprintf(str, "%s", TYPE_STRS[type]);
    else
        sprintf(str, "%s[%d]", TYPE_STRS[type], size);
}

void compile_node(pAstNode p) {
    if(p == NULL) return;
    //fprintf(stderr, "%s(%s)\n", __func__, ASTN_STRS[p->base.node_type]);
    switch(p->base.node_type) {
        case ASTN_BOp: {
            BOpNode* q = (BOpNode*)p;
            compile_node(q->arg1);
            compile_node(q->arg2);
            valtype_t type1 = q->arg1->base.type, type2 = q->arg2->base.type;
            q->base.size = 0;
            int size1 = q->arg1->base.size, size2 = q->arg2->base.size;
            if(size1 == 0 && size2 == 0) {
                q->base.type = get_composite_type(q->op, type1, type2, q->base.line, q->base.col);
            }
            else {
                if(size1 > 0) {
                    print_error("type", ERROR, 12, q->arg1->base.line, q->arg1->base.col,
                        OP_STRS[q->op], NULL, "Left operand cannot be an array.");
                }
                if(size2 > 0) {
                    print_error("type", ERROR, 13, q->arg2->base.line, q->arg2->base.col,
                        OP_STRS[q->op], NULL, "Right operand cannot be an array.");
                }
                q->base.type = TYPE_ERROR;
            }
            break;
        }
        case ASTN_UOp: {
            UOpNode* q = (UOpNode*)p;
            compile_node(q->arg);
            valtype_t subtype = q->arg->base.type;
            q->base.size = 0;
            int subsize = q->arg->base.size;
            if(subsize == 0) {
                if(subtype == TYPE_INTEGER || subtype == TYPE_REAL) {
                    q->base.type = subtype;
                }
                else if(subtype == TYPE_ERROR) {
                    q->base.type = TYPE_ERROR;
                }
                else {
                    q->base.type = TYPE_ERROR;
                    sprintf(msg, "Operand's type should be INTEGER or REAL, not %s.", TYPE_STRS[subtype]);
                    print_error("type", ERROR, 14, q->arg->base.line, q->arg->base.col,
                        OP_STRS[q->op], NULL, msg);
                }
            }
            else {
                print_error("type", ERROR, 15, q->arg->base.line, q->arg->base.col,
                    OP_STRS[q->op], NULL, "Operand cannot be an array.");
                q->base.type = TYPE_ERROR;
            }
            break;
        }
        case ASTN_Deref: {
            DerefNode* q = (DerefNode*)p;
            compile_node(q->index);
            q->base.size = 0;
            pSTEntry entry = ST_get_entry(&mySTStack, q->varname);
            if(entry == NULL) {
                print_undecl_id_error(q->varname, q->base.line, q->base.col);
                q->base.type = TYPE_ERROR;
            }
            else if(entry->size == 0) {
                print_error("type", ERROR, 16, q->base.line, q->base.col, q->varname, "NONARR_DEREF",
                    "Dereference operator applied on a variable which is not an array.");
                q->base.type = TYPE_ERROR;
            }
            else {
                q->base.type = entry->type;
                int size = entry->size;
                if(q->index->base.node_type == ASTN_Var) {
                    if(!(q->index->base.type == TYPE_INTEGER && q->index->base.size == 0)) {
                        if(q->index->base.size > 0) {
                            sprintf(msg, "Array index should be an integer, not %s[%d].",
                                TYPE_STRS[q->index->base.type], q->index->base.size);
                        }
                        else {
                            sprintf(msg, "Array index should be an integer, not %s.",
                                TYPE_STRS[q->index->base.type]);
                        }
                        print_error("type", ERROR, 17, q->base.line, q->base.col, q->varname, "NONINT_INDEX", msg);
                    }
                }
                else {
                    int val = ((NumNode*)(q->index))->val;
                    if(val <= 0 || val > size) {
                        sprintf(msg, "Index of %s should be in the range 1 to %d.\n", q->varname, entry->size);
                        print_error("type", ERROR, 18, q->index->base.line, q->index->base.col, NULL, "OOB_INDEX", msg);
                    }
                }
            }
            break;
        }
        case ASTN_Var: {
            VarNode* q = (VarNode*)p;
            pSTEntry entry = ST_get_entry(&mySTStack, q->varname);
            if(entry == NULL) {
                print_undecl_id_error(q->varname, q->base.line, q->base.col);
                q->base.type = TYPE_ERROR;
                q->base.size = 0;
            }
            else {
                q->base.type = entry->type;
                q->base.size = entry->size;
            }
            break;
        }
        case ASTN_Num:
        case ASTN_RNum:
        case ASTN_Bool:
        case ASTN_Range:
            break;
        case ASTN_Module: {
            ModuleNode* q = (ModuleNode*)p;
            compile_node_chain(q->body);
            break;
        }
        case ASTN_Assn: {
            AssnNode* q = (AssnNode*)p;
            compile_node(q->target);
            compile_node(q->expr);
            valtype_t type1 = q->target->base.type, type2 = q->expr->base.type;
            int size1 = q->target->base.size, size2 = q->expr->base.size;
            if((type1 != type2 || size1 != size2) && type2 != TYPE_ERROR) {
                char tstr1[24], tstr2[24];
                get_type_str(tstr1, type1, size1);
                get_type_str(tstr2, type2, size2);
                sprintf(msg, "Type of target is %s, but type of expression is %s.", tstr1, tstr2);
                print_error("type", ERROR, 19, q->base.line, q->base.col, NULL, NULL, msg);
            }
            break;
        }
        case ASTN_While: {
            WhileNode* q = (WhileNode*)p;
            compile_node(q->cond);
            int type = q->cond->base.type;
            int size = q->cond->base.size;
            if(!(q->cond->base.type == TYPE_BOOLEAN && q->cond->base.size == 0)) {
                char tstr[24];
                get_type_str(tstr, type, size);
                sprintf(msg, "While loop's condition has type %s instead of BOOLEAN.", tstr);
                print_error("type", ERROR, 20, q->cond->base.line, q->cond->base.col, NULL, "NONBOOL_COND", msg);
            }
            compile_node_chain(q->body);
            break;
        }
        case ASTN_For:
            compile_node_chain(((ForNode*)p)->body);
            break;
        case ASTN_Decl: {
            DeclNode* q = (DeclNode*)p;
            IDListNode* node = (IDListNode*)(q->idList);
            while(node != NULL) {
                pSTEntry entry = malloc(sizeof(STEntry));
#ifdef LOG_MEM
                fprintf(stderr, "%s: Allocated STEntry %p\n", __func__, (void*)entry);
#endif
                entry->type = q->base.type;
                entry->size = q->base.size;
                entry->line = node->base.line;
                entry->col = node->base.col;
                entry->offset = 0;
                entry->lexeme = node->varname;
                ST_add_entry(&mySTStack, entry);
                //free(entry);
#ifdef LOG_MEM
                fprintf(stderr, "%s: Freed STEntry %p\n", __func__, (void*)entry);
#endif
                node = node->next;
            }
            break;
        }
        case ASTN_Input:
            break;
        case ASTN_Output:
            compile_node(((OutputNode*)p)->var);
            break;
        case ASTN_FCall:
            break;
        case ASTN_Switch: {
            SwitchNode* q = (SwitchNode*)p;
            compile_node_chain((pAstNode)(q->cases));
            compile_node((pAstNode)(q->defaultcase));
            break;
        }
        case ASTN_Case: {
            CaseNode* q = (CaseNode*)p;
            compile_node(q->val);
            compile_node_chain(q->stmts);
            break;
        }
        default:
            complain_ast_node_type(__func__, p->base.node_type);
    }
}

void compile(ProgramNode* root) {
    vptr_int_hmap_init(&module_status, 10, false);
    vptr_pMN_hmap_init(&module_node_map, 10, false);

    IDListNode* decl_node = NULL;
    ModuleNode* module_node = NULL;

    decl_node = root->decls;
    while(decl_node != NULL) {
        int old_size = module_status.size;
        vptr_int_hmap_insert(&module_status, decl_node->varname, MODULE_DECLARED);
        if(module_status.size == old_size) {
            sprintf(msg, "Module '%s' has already been declared.", decl_node->varname);
            print_error("compile", ERROR, 1, decl_node->base.line, decl_node->base.col, NULL, "MODULE_REDECL", msg);
        }
        decl_node = decl_node->next;
    }

    module_node = root->modules;
    while(module_node != NULL) {
        if(module_node->name != NULL) {
            vptr_int_hmap_insert(&module_status, module_node->name, 0);
            int old_size = module_node_map.size;
            ModuleNode* n2 = vptr_pMN_hmap_insert(&module_node_map, module_node->name, module_node)->value;
            if(module_node_map.size == old_size) {
                sprintf(msg, "Module '%s' has already been defined on line %d col %d.",
                    module_node->name, n2->base.line, n2->base.col);
                print_error("compile", ERROR, 2, module_node->base.line, module_node->base.col, NULL, "MODULE_REDEF", msg);
            }
        }
        module_node = module_node->next;
    }

    module_node = root->modules;
    while(module_node != NULL) {
        if(module_node->name != NULL) {
            vptr_int_hmap_insert(&module_status, module_node->name, 0)->value |= MODULE_DEFINED;
        }
        ST_reinit(&mySTStack, module_node->name);
        compile_node((pAstNode)module_node);
        STStack_clear(&mySTStack);
        module_node = module_node->next;
    }

    //vptr_pMN_hmap_print(&module_node_map, stderr);
    //vptr_int_hmap_print(&module_status, stderr);

    decl_node = root->decls;
    while(decl_node != NULL) {
        int status = vptr_int_hmap_get(&module_status, decl_node->varname);
        if((status & MODULE_DECLARED) && (~status & MODULE_DEFINED)) {
            // module is declared but not defined
            sprintf(msg, "Module '%s' has been declared but not defined.", decl_node->varname);
            print_error("compile", ERROR, 3, decl_node->base.line, decl_node->base.col, NULL, "MODULE_NOTDECL", msg);
        }
        decl_node = decl_node->next;
    }

    module_node = root->modules;
    while(module_node != NULL) {
        if(module_node->name != NULL) {
            int status = vptr_int_hmap_get(&module_status, module_node->name);
            if(~status & MODULE_USED) {
                sprintf(msg, "Module '%s' has not been used.", module_node->name);
                print_error("compile", WARNING, 4, module_node->base.line, module_node->base.col, NULL, "MODULE_NOTUSED", msg);
            }
        }
        module_node = module_node->next;
    }

    vptr_int_hmap_clear(&module_status);
    vptr_pMN_hmap_clear(&module_node_map);
    STStack_clear(&mySTStack);
}

int compiler_main(FILE* ifp, FILE* ofp, int verbosity) {
    gsymb_t start_symb = init_parser();

    parse_tree_node* proot = build_parse_tree(ifp, start_symb);
    build_ast(proot);
    ProgramNode* ast = (ProgramNode*)proot->value->tree;
    if(ast->base.node_type != ASTN_Program) {
        fprintf(stderr, "Root node of AST is %s instead of ProgramNode", ASTN_STRS[ast->base.node_type]);
        abort();
    }
    parse_tree_destroy(proot);
    destroy_parser(false);

    compile(ast);

    destroy_ast((pAstNode)ast);
    pch_int_hmap_clear(&intern_table);
    return 0;
}

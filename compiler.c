/* Compiles AST to IRCode */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "parser.h"
#include "error.h"
#include "ast.h"
#include "ast_gen.h"
#include "symbol_table.h"
#include "util/vptr_int_hmap.h"
#include "type.h"
#include "compiler.h"
#include "codegen.h"
#include "compile_x86.h"

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

static char msg[200];

void print_type_error(op_t op, valtype_t type1, valtype_t type2, int line, int col) {
    sprintf(msg, "Operands are of the wrong type ('%s' and '%s') for operator '%s'.",
        TYPE_STRS[type1], TYPE_STRS[type2], OP_STRS[op]);
    print_error("type", ERROR, -1, line, col, NULL, "BAD_TYPE_FOR_OP", msg);
}

void print_undecl_id_error(const char* varname, int line, int col) {
    print_error("semantic", ERROR, -1, line, col, varname, "UNDECL_ID", "Undeclared identifier");
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
        case OP_DIV:
            if(type1 == TYPE_INTEGER || type1 == TYPE_REAL)
                return type1;
            else
                print_type_error(op, type1, type2, line, col);
            break;
        /*
        case OP_DIV:
            if(type1 == TYPE_INTEGER || type1 == TYPE_REAL)
                return TYPE_REAL;
            else
                print_type_error(op, type1, type2, line, col);
            break;
        */
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

void compile_node(pAstNode p, pSD psd, const char* func_name);

void compile_node_chain(pAstNode p, pSD psd, const char* func_name) {
    while(p != NULL) {
        compile_node(p, psd, func_name);
        p = get_next_ast_node(p);
    }
}

AddrNode* add_node_addr_to_SD(pAstNode node, pSD psd, addr_type_t addr_type) {
    AddrNode* anode = malloc(sizeof(AddrNode));
#ifdef LOG_MEM
    fprintf(stderr, "%s: Allocated AddrNode %p\n", __func__, (void*)anode);
#endif
    anode->next = NULL;
    anode->symbol_table = (psd->active)->value;
    anode->id = 0;
    anode->addr_type = addr_type;
    anode->type = node->base.type;
    anode->size = node->base.size;
    anode->line = node->base.line;
    anode->col = node->base.col;
    anode->offset = 0;
    SD_add_addr(psd, anode);
    return anode;
}

void add_node_var_to_SD(pAstNode node, pSD psd, const char* func_name) {
    pSTEntry entry = malloc(sizeof(STEntry));
#ifdef LOG_MEM
    fprintf(stderr, "%s: Allocated STEntry %p\n", __func__, (void*)entry);
#endif
    entry->func_name = func_name;
    entry->use_line = 0;
    entry->use_col = 0;
    entry->readonly = false;
    entry->next = NULL;
    entry->addr = NULL;
    if(node->base.node_type == ASTN_IDList) {
        entry->lexeme = ((IDListNode*)node)->varname;
    }
    else if(node->base.node_type == ASTN_IDTypeList) {
        entry->lexeme = ((IDTypeListNode*)node)->varname;
    }
    else {
        entry->lexeme = NULL;
    }
    int line = node->base.line, col = node->base.col;
    if(!SD_add_entry(psd, entry, line, col)) {
        free(entry);
#ifdef LOG_MEM
        fprintf(stderr, "%s: Freed STEntry %p\n", __func__, (void*)entry);
#endif
    }
    else {
        entry->addr = add_node_addr_to_SD(node, psd, ADDR_VAR);
    }
}

void log_var_set(pSTEntry entry, int line, int col) {
    if(entry->readonly == true) {
        sprintf(msg, "Cannot modify read-only variable '%s' (declared at line %d, col %d).",
            entry->lexeme, entry->addr->line, entry->addr->col);
        print_error("type", ERROR, -1, line, col, entry->lexeme, "RDONLY_MOD", msg);
    }
    entry->use_line = line;
    entry->use_col = col;
}

void compare_lists_type(const char* func_name, pSD psd, bool is_output, IDListNode* actual, IDTypeListNode* formal) {
    const char* list_type = is_output? "output": "input";
    int acount=0, fcount=0;
    IDListNode* org_actual = actual;
    IDTypeListNode* org_formal = formal;
    for(; actual != NULL; actual = actual->next, acount++);
    for(; formal != NULL; formal = formal->next, fcount++);
    actual = org_actual;
    formal = org_formal;
    if(acount < fcount) {
        sprintf(msg, "Too few %s parameters to function.", list_type);
        print_error("type", ERROR, -1, actual->base.line, actual->base.col, func_name,
            "FEW_ARGS", msg);
    }
    else if(acount > fcount) {
        sprintf(msg, "Too many %s parameters to function.", list_type);
        print_error("type", ERROR, -1, actual->base.line, actual->base.col, func_name,
            "MANY_ARGS", msg);
    }
    else {
        while(actual != NULL) {
            int ftype = formal->base.type;
            int fsize = formal->base.size;
            pSTEntry entry = SD_get_entry(psd, actual->varname);
            if(entry == NULL) {
                print_undecl_id_error(actual->varname, actual->base.line, actual->base.col);
            }
            else {
                if(is_output) {
                    log_var_set(entry, actual->base.line, actual->base.col);
                }
                int atype = entry->addr->type;
                int asize = entry->addr->size;
                if(!(atype == TYPE_ERROR || (atype == ftype && asize == fsize))) {
                    char fstr[24], astr[24];
                    get_type_str(astr, atype, asize);
                    get_type_str(fstr, ftype, fsize);
                    sprintf(msg, "Actual %s parameter has type '%s', but formal %s parameter has type '%s'.",
                        list_type, astr, list_type, fstr);
                    print_error("type", ERROR, -1, actual->base.line, actual->base.col, actual->varname,
                        "FCALL_TYPE_MISMATCH", msg);
                }
                actual->base.addr = entry->addr;
            }
            actual = actual->next;
            formal = formal->next;
        }
    }
}

void compile_node(pAstNode p, pSD psd, const char* func_name) {
    if(p == NULL) return;
    //fprintf(stderr, "%s(%s)\n", __func__, ASTN_STRS[p->base.node_type]);
    switch(p->base.node_type) {
        case ASTN_BOp: {
            BOpNode* q = (BOpNode*)p;
            compile_node(q->arg1, psd, func_name);
            compile_node(q->arg2, psd, func_name);
            valtype_t type1 = q->arg1->base.type, type2 = q->arg2->base.type;
            q->base.size = 0;
            int size1 = q->arg1->base.size, size2 = q->arg2->base.size;
            if(size1 == 0 && size2 == 0) {
                q->base.type = get_composite_type(q->op, type1, type2, q->base.line, q->base.col);
                if(q->base.type != TYPE_ERROR) {
                    q->base.addr = add_node_addr_to_SD(p, psd, ADDR_TEMP);
                }
            }
            else {
                if(size1 > 0) {
                    print_error("type", ERROR, -1, q->arg1->base.line, q->arg1->base.col,
                        OP_STRS[q->op], "LOP_ARRAY", "Left operand cannot be an array.");
                }
                if(size2 > 0) {
                    print_error("type", ERROR, -1, q->arg2->base.line, q->arg2->base.col,
                        OP_STRS[q->op], "ROP_ARRAY", "Right operand cannot be an array.");
                }
                q->base.type = TYPE_ERROR;
            }
            break;
        }
        case ASTN_UOp: {
            UOpNode* q = (UOpNode*)p;
            compile_node(q->arg, psd, func_name);
            valtype_t subtype = q->arg->base.type;
            q->base.size = 0;
            int subsize = q->arg->base.size;
            if(subsize == 0) {
                if(subtype == TYPE_INTEGER || subtype == TYPE_REAL) {
                    q->base.type = subtype;
                    q->base.addr = add_node_addr_to_SD(p, psd, ADDR_TEMP);
                }
                else if(subtype == TYPE_ERROR) {
                    q->base.type = TYPE_ERROR;
                }
                else {
                    q->base.type = TYPE_ERROR;
                    sprintf(msg, "Operand's type should be '%s' or '%s', not '%s'.",
                        TYPE_STRS[TYPE_INTEGER], TYPE_STRS[TYPE_REAL], TYPE_STRS[subtype]);
                    print_error("type", ERROR, -1, q->arg->base.line, q->arg->base.col,
                        OP_STRS[q->op], "UOP_BAD_TYPE", msg);
                }
            }
            else {
                print_error("type", ERROR, -1, q->arg->base.line, q->arg->base.col,
                    OP_STRS[q->op], "UOP_ARRAY", "Operand cannot be an array.");
                q->base.type = TYPE_ERROR;
            }
            break;
        }
        case ASTN_Deref: {
            DerefNode* q = (DerefNode*)p;
            compile_node(q->index, psd, func_name);
            q->base.size = 0;
            pSTEntry entry = SD_get_entry(psd, q->varname);
            if(entry == NULL) {
                print_undecl_id_error(q->varname, q->base.line, q->base.col);
                q->base.type = TYPE_ERROR;
            }
            else if(entry->addr->size == 0) {
                print_error("type", ERROR, -1, q->base.line, q->base.col, q->varname, "NONARR_DEREF",
                    "Dereference operator applied on a variable which is not an array.");
                q->base.type = TYPE_ERROR;
            }
            else {
                q->base.type = entry->addr->type;
                int size = entry->addr->size;
                q->base.addr = add_node_addr_to_SD(p, psd, ADDR_ARR);
                q->base.addr->base_addr = entry->addr;
                q->base.addr->index = q->index->base.addr;
                if(q->index->base.node_type == ASTN_Var) {
                    int type = q->index->base.type;
                    if(!((type == TYPE_INTEGER || type == TYPE_ERROR) && q->index->base.size == 0)) {
                        char tstr[20];
                        get_type_str(tstr, q->index->base.type, q->index->base.size);
                        sprintf(msg, "Array index should be an integer, not '%s'.", tstr);
                        print_error("type", ERROR, -1, q->base.line, q->base.col, q->varname, "NONINT_INDEX", msg);
                    }
                }
                else {
                    int val = ((NumNode*)(q->index))->val;
                    if(val <= 0 || val > size) {
                        sprintf(msg, "Index of '%s' should be in the range 1 to %d.", q->varname, entry->addr->size);
                        print_error("type", ERROR, -1, q->index->base.line, q->index->base.col, NULL, "OOB_INDEX", msg);
                    }
                }
            }
            break;
        }
        case ASTN_Var: {
            VarNode* q = (VarNode*)p;
            pSTEntry entry = SD_get_entry(psd, q->varname);
            if(entry == NULL) {
                print_undecl_id_error(q->varname, q->base.line, q->base.col);
                q->base.type = TYPE_ERROR;
                q->base.size = 0;
            }
            else {
                q->base.type = entry->addr->type;
                q->base.size = entry->addr->size;
                q->base.addr = entry->addr;
            }
            break;
        }
        case ASTN_Num: {
            NumNode* q = (NumNode*)p;
            q->base.addr = add_node_addr_to_SD(p, psd, ADDR_CONST);
            q->base.addr->imm.i = q->val;
            break;
        }
        case ASTN_RNum: {
            RNumNode* q = (RNumNode*)p;
            q->base.addr = add_node_addr_to_SD(p, psd, ADDR_CONST);
            q->base.addr->imm.f = q->val;
            break;
        }
        case ASTN_Bool: {
            BoolNode* q = (BoolNode*)p;
            q->base.addr = add_node_addr_to_SD(p, psd, ADDR_CONST);
            q->base.addr->imm.b = q->val;
            break;
        }
        case ASTN_Module: {
            ModuleNode* q = (ModuleNode*)p;
            if(q->name != NULL) {
                vptr_int_hmap_node* hmap_node = vptr_int_hmap_insert(&module_status, q->name, 0);
                hmap_node->value |= MODULE_DEFINED;
                int status = hmap_node->value;
                if((status & MODULE_DECLARED) && (~status & MODULE_USED)) {
                    print_error("semantic", WARNING, 6, q->base.line, q->base.col, q->name,
                        "NEEDLESS_DECLARE", "Module has been declared but not used before defining it.");
                }
            }
            IDTypeListNode* node = NULL;
            node = q->iParamList;
            while(node != NULL) {
                add_node_var_to_SD((pAstNode)node, psd, func_name);
                node = node->next;
            }
            node = q->oParamList;
            while(node != NULL) {
                add_node_var_to_SD((pAstNode)node, psd, func_name);
                node = node->next;
            }
            compile_node_chain(q->body, psd, func_name);
            node = q->oParamList;
            while(node != NULL) {
                pSTEntry entry = SD_get_entry(psd, node->varname);
                if(entry->use_line == 0) {
                    print_error("semantic", ERROR, -1, node->base.line, node->base.col, node->varname,
                        "UNSET_OUTPUT_PARAM", "Output parameter has not been set.");
                }
                node = node->next;
            }
            break;
        }
        case ASTN_Assn: {
            AssnNode* q = (AssnNode*)p;
            compile_node(q->target, psd, func_name);
            compile_node(q->expr, psd, func_name);
            valtype_t type1 = q->target->base.type, type2 = q->expr->base.type;
            int size1 = q->target->base.size, size2 = q->expr->base.size;
            if((type1 != type2 || size1 != size2) && type2 != TYPE_ERROR && type1 != TYPE_ERROR) {
                char tstr1[24], tstr2[24];
                get_type_str(tstr1, type1, size1);
                get_type_str(tstr2, type2, size2);
                sprintf(msg, "Type of target is '%s', but type of expression is '%s'.", tstr1, tstr2);
                print_error("type", ERROR, -1, q->base.line, q->base.col, NULL, "ASSN_TYPE_MISMATCH", msg);
            }
            const char* varname = NULL;
            if(q->target->base.node_type == ASTN_Deref) {
                varname = ((DerefNode*)(q->target))->varname;
            }
            else if(q->target->base.node_type == ASTN_Var) {
                varname = ((VarNode*)(q->target))->varname;
            }
            pSTEntry entry = SD_get_entry(psd, varname);
            if(entry != NULL) {
                log_var_set(entry, q->target->base.line, q->target->base.col);
            }
            break;
        }
        case ASTN_While: {
            WhileNode* q = (WhileNode*)p;
            compile_node(q->cond, psd, func_name);
            int type = q->cond->base.type;
            int size = q->cond->base.size;
            int cond_type = q->cond->base.type;
            if(!((cond_type == TYPE_BOOLEAN || cond_type == TYPE_ERROR) && q->cond->base.size == 0)) {
                char tstr[24];
                get_type_str(tstr, type, size);
                sprintf(msg, "While loop's condition has type '%s' instead of '%s'.", tstr, TYPE_STRS[TYPE_BOOLEAN]);
                print_error("type", ERROR, -1, q->cond->base.line, q->cond->base.col, NULL, "NONBOOL_COND", msg);
            }
            SD_add_scope(psd, p);
            compile_node_chain(q->body, psd, func_name);
            SD_remove_scope(psd);
            break;
        }
        case ASTN_For: {
            ForNode* q = (ForNode*)p;
            pSTEntry entry = SD_get_entry(psd, q->varname);
            if(entry == NULL) {
                print_undecl_id_error(q->varname, q->base.line, q->base.col);
            }
            else if(!(entry->addr->type == TYPE_INTEGER && entry->addr->size == 0)) {
                char tstr[24];
                get_type_str(tstr, entry->addr->type, entry->addr->size);
                sprintf(msg, "Type of loop variable '%s' should be '%s', not '%s'.",
                    q->varname, TYPE_STRS[TYPE_INTEGER], tstr);
                print_error("type", ERROR, -1, q->base.line, q->base.col, NULL, "LOOPVAR_NOTINT", msg);
            }
            bool prev_readonly = false;
            int prev_line = 0, prev_col = 0;
            if(entry != NULL) {
                prev_readonly = entry->readonly;
                prev_line = entry->addr->line;
                prev_col = entry->addr->col;
                entry->readonly = true;
                entry->addr->line = q->base.line;
                entry->addr->col = q->base.col;
                q->base.type = entry->addr->type;
                q->base.addr = entry->addr;
                q->beg_addr = add_node_addr_to_SD(p, psd, ADDR_CONST);
                q->beg_addr->imm.i = q->beg;
                q->end_addr = add_node_addr_to_SD(p, psd, ADDR_CONST);
                q->end_addr->imm.i = q->end;
            }
            if(prev_readonly) {
                pSTEntry entry = SD_get_entry(psd, q->varname);
                if(entry != NULL) {
                    log_var_set(entry, q->base.line, q->base.col);
                }
            }
            SD_add_scope(psd, p);
            compile_node_chain(((ForNode*)p)->body, psd, func_name);
            SD_remove_scope(psd);
            if(entry != NULL) {
                entry->readonly = prev_readonly;
                entry->addr->line = prev_line;
                entry->addr->col = prev_col;
            }
            break;
        }
        case ASTN_Decl: {
            DeclNode* q = (DeclNode*)p;
            IDListNode* node = (IDListNode*)(q->idList);
            while(node != NULL) {
                node->base.type = q->base.type;
                node->base.size = q->base.size;
                add_node_var_to_SD((pAstNode)node, psd, func_name);
                node = node->next;
            }
            break;
        }
        case ASTN_Input: {
            InputNode* q = (InputNode*)p;
            pSTEntry entry = SD_get_entry(psd, q->varname);
            if(entry == NULL) {
                print_undecl_id_error(q->varname, q->base.line, q->base.col);
                q->base.type = TYPE_ERROR;
                q->base.size = 0;
            }
            else {
                q->base.type = entry->addr->type;
                q->base.size = entry->addr->size;
                q->base.addr = entry->addr;
            }
            break;
        }
        case ASTN_Output:
            compile_node(((OutputNode*)p)->var, psd, func_name);
            break;
        case ASTN_FCall: {
            FCallNode* q = (FCallNode*)p;
            if(q->name == func_name) {
                print_error("semantic", ERROR, -1, q->base.line, q->base.col, q->name, "RECURSE_CALL",
                    "Recursive calls are not allowed.");
            }
            vptr_pMN_hmap_node* hmap_node = vptr_pMN_hmap_query(&module_node_map, q->name);
            if(hmap_node == NULL) {
                print_undecl_id_error(q->name, q->base.line, q->base.col);
            }
            else {
                ModuleNode* module_node = hmap_node->value;
                vptr_int_hmap_node* hmap_node = vptr_int_hmap_insert(&module_status, q->name, 0);
                hmap_node->value |= MODULE_USED;
                if(!((hmap_node->value & MODULE_DEFINED) || (hmap_node->value & MODULE_DECLARED))) {
                    print_error("semantic", ERROR, -1, q->base.line, q->base.col, q->name, "UNDEF_MODULE",
                        "Module must be defined or declared before using it.");
                }
                compare_lists_type(q->name, psd, false, q->iParamList, module_node->iParamList);
                if(q->oParamList != NULL) {
                    compare_lists_type(q->name, psd, true, q->oParamList, module_node->oParamList);
                }
            }
            break;
        }
        case ASTN_Switch: {
            SwitchNode* q = (SwitchNode*)p;
            pSTEntry entry = SD_get_entry(psd, q->varname);
            int type = TYPE_ERROR;
            char tstr[24] = "ERROR", tstr2[24];
            if(entry == NULL) {
                print_undecl_id_error(q->varname, q->base.line, q->base.col);
            }
            else {
                q->base.addr = entry->addr;
                if(entry->addr->size > 0 || (entry->addr->type != TYPE_INTEGER && entry->addr->type != TYPE_BOOLEAN)) {
                    get_type_str(tstr2, entry->addr->type, entry->addr->size);
                    sprintf(msg, "Switch variable's type should be '%s' or '%s', not '%s'",
                        TYPE_STRS[TYPE_INTEGER], TYPE_STRS[TYPE_BOOLEAN], tstr2);
                    print_error("type", ERROR, -1, q->base.line, q->base.col, q->varname, "SWITCH_BAD_TYPE", msg);
                }
                else {
                    type = entry->addr->type;
                    get_type_str(tstr, type, 0);
                }
            }
            CaseNode* node = q->cases;
            int count[2] = {0, 0};
            SD_add_scope(psd, p);
            while(node != NULL) {
                compile_node(node->val, psd, func_name);
                int type2 = node->val->base.type;
                int size2 = node->val->base.size;
                if(size2 > 0 || (type2 != type && type != TYPE_ERROR)) {
                    get_type_str(tstr2, type2, size2);
                    sprintf(msg, "Switch variable has type '%s', but case variable has type '%s'.", tstr, tstr2);
                    print_error("type", ERROR, -1, node->val->base.line, node->val->base.col, NULL, "SWITCH_TYPE_MISMATCH", msg);
                }
                else if(type == TYPE_BOOLEAN) {
                    if(node->val->base.node_type == ASTN_Bool) {
                        bool val = ((BoolNode*)(node->val))->val;
                        count[val & 1]++;
                        if(count[val & 1] > 1) {
                            print_error("type", ERROR, -1, node->val->base.line, node->val->base.col,
                                NULL, "SWITCH_DUP", "Duplicate value in switch statement.");
                        }
                    }
                    else {
                        fprintf(stderr, "Assert failure in boolean switch.\n");
                    }
                }
                compile_node_chain((pAstNode)(node->stmts), psd, func_name);
                node = node->next;
            }
            if(type == TYPE_BOOLEAN) {
                if(count[0] == 0) {
                    print_error("type", ERROR, -1, p->base.line, p->base.col,
                        NULL, "SWITCH_NOFALSE", "The case 'false' hasn't been handled.");
                }
                if(count[1] == 0) {
                    print_error("type", ERROR, -1, p->base.line, p->base.col,
                        NULL, "SWITCH_NOTRUE", "The case 'true' hasn't been handled.");
                }
            }
            if(q->defaultcase != NULL) {
                if(type == TYPE_BOOLEAN) {
                    print_error("type", ERROR, -1, q->defaultcase->base.line, q->defaultcase->base.col,
                        NULL, "SWITCH_BOOLDEFAULT", "Default case is not allowed when switch variable is boolean.");
                }
                compile_node_chain((pAstNode)(q->defaultcase->stmts), psd, func_name);
            }
            else if(type == TYPE_INTEGER) {
                print_error("type", ERROR, -1, q->base.line, q->base.col, NULL, "SWITCH_INT_DEFAULT",
                    "Default case is mandatory when switch variable is an integer.");
            }
            SD_remove_scope(psd);
            break;
        }
        default:
            complain_ast_node_type(__func__, p->base.node_type);
    }
    codegen(p);
}

void compile_program(ProgramNode* root, pSD psd, bool code_gen, bool destroy_module_bodies) {
    if(code_gen) {
        allow_codegen = true;
    }

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
            print_error("semantic", ERROR, -1, decl_node->base.line, decl_node->base.col, NULL, "MODULE_REDECL", msg);
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
                print_error("semantic", ERROR, -1, module_node->base.line, module_node->base.col, NULL, "MODULE_REDEF", msg);
            }
        }
        module_node = module_node->next;
    }

    module_node = root->modules;
    while(module_node != NULL) {
        SD_add_scope(psd, (pAstNode)module_node);
        compile_node((pAstNode)module_node, psd, module_node->name);
        SD_remove_scope(psd);
        if(destroy_module_bodies) {
            destroy_ast(module_node->body);
        }
        optimize_ircode(&(module_node->base.ircode));
        module_node->body = NULL;
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
            print_error("semantic", ERROR, -1, decl_node->base.line, decl_node->base.col, NULL, "MODULE_NOTDECL", msg);
        }
        decl_node = decl_node->next;
    }

    module_node = root->modules;
    while(module_node != NULL) {
        if(module_node->name != NULL) {
            int status = vptr_int_hmap_get(&module_status, module_node->name);
            if(~status & MODULE_USED) {
                sprintf(msg, "Module '%s' has not been used.", module_node->name);
                print_error("semantic", WARNING, 4, module_node->base.line, module_node->base.col, NULL, "MODULE_NOTUSED", msg);
            }
        }
        module_node = module_node->next;
    }

    vptr_int_hmap_clear(&module_status);
    vptr_pMN_hmap_clear(&module_node_map);
}

int compiler_main(FILE* ifp, FILE* ofp, int level, int verbosity) {
    /*  level=0: semantic check only
        level=1: print symbol database entries
        level=2: allow_codegen
        level=3: x86_code_gen
    */
    gsymb_t start_symb = init_parser();

    parse_tree_node* proot = build_parse_tree(ifp, start_symb);
    int parse_errors = error_count;
    ProgramNode* ast = NULL;
    SD mySD;

    if(parse_errors == 0) {
        build_ast(proot);
        ast = (ProgramNode*)proot->value->tree;
        if(ast->base.node_type != ASTN_Program) {
            fprintf(stderr, "Root node of AST is %s instead of ProgramNode", ASTN_STRS[ast->base.node_type]);
            abort();
        }
    }
    parse_tree_destroy(proot);
    destroy_parser(false);

    bool print_sd = false;
    bool print_entry_list = false;
    if((level==1) > 0) {
        print_entry_list = true;
    }
    if(verbosity > 0) {
        print_sd = true;
    }
    if(parse_errors == 0) {
        SD_init(&mySD);
        compile_program(ast, &mySD, level>=2, true);
        if(print_sd) {
            SD_print(&mySD, stderr);
        }
        if(print_entry_list) {
            SD_print_sub(&mySD, stderr);
        }
        ModuleNode* node = ast->modules;
        while(node != NULL) {
            if(error_count == 0 && level == 2) {
                fprintf(ofp, "Intermediate code for ");
                if(node->name == NULL) {
                    fprintf(ofp, "driver:\n");
                }
                else {
                    fprintf(ofp, "module %s:\n", node->name);
                }
                ircode_print(&(node->base.ircode), ofp);
            }
            node = node->next;
        }

        if(error_count == 0 && level == 3) {
            compile_program_to_x86(ast, &mySD, ofp);
        }
        node = ast->modules;
        while(node != NULL) {
            ircode_clear(&(node->base.ircode));
            node = node->next;
        }
        destroy_ast((pAstNode)ast);
        SD_clear(&mySD);
    }

    pch_int_hmap_clear(&intern_table);
    return 0;
}

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

static void pMN_destroy(pMN p){}
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

void compile_module(ModuleNode* module_node) {
    if(module_node->name != NULL) {
        vptr_int_hmap_insert(&module_status, module_node->name, 0)->value |= MODULE_DEFINED;
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
        compile_module(module_node);
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

    vptr_int_hmap_destroy(&module_status);
    vptr_pMN_hmap_destroy(&module_node_map);
    STStack_destroy(&mySTStack);
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
    pch_int_hmap_destroy(&intern_table);
    return 0;
}

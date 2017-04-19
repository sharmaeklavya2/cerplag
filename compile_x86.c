/* Compiles IRCode to x86 assembly */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ircode.h"
#include "addr.h"
#include "ast.h"
#include "x86.h"

void optimize_ircode(IRCode* code) {
}

void optimize_x86_code(X86Code* code) {
}

void compile_x86_error(const char* msg) {
    fprintf(stderr, "x86 compile error: %s", msg);
}

void addr_to_x86_arg(AddrNode* an, char* dest) {
    switch(an->addr_type) {
        case ADDR_VAR:
            snprintf(dest, X86_ARG_SIZE, "data");
            break;
        case ADDR_TEMP:
        case ADDR_CONST:
        case ADDR_ARR:
            break;
    }
}

void compile_instr_to_x86(IRInstr* inode, X86Code* ocode) {
    switch(inode->op) {
        case OP_MOV:
            x86_code_append(ocode, x86_instr_new(X86_OP_mov));
            break;
        case OP_PLUS:
            x86_code_append(ocode, x86_instr_new(X86_OP_add));
        default:
            break;
    }
}

void compile_code_to_x86(IRCode* icode, X86Code* ocode) {
    IRInstr* inode = icode->first;
    for(; inode != NULL; inode = inode->next) {
        compile_instr_to_x86(inode, ocode);
    }
}

const char PROLOGUE_STR[] =
    X86_INDENT_STR "global main\n"
    X86_INDENT_STR "extern printf\n"
    X86_INDENT_STR "extern scanf\n"
    "section .text\n";

const char EPILOGUE_STR[] =
    "section .bss\n"
    "data:\n"
    X86_INDENT_STR "resb %d\n";

void compile_program_to_x86(ProgramNode* program_node, pSD psd, FILE* ofp) {
    int data_size = psd->root->value->max_offset;
    fprintf(ofp, PROLOGUE_STR);

    X86Code x86_code;
    x86_code_init(&x86_code);
    ModuleNode* module_node = program_node->modules;
    for(; module_node != NULL; module_node = module_node->next) {
        optimize_ircode(&(module_node->base.ircode));
        if(module_node->name == NULL) {
            fprintf(ofp, "main:\n");
        }
        else {
            fprintf(ofp, "module_%s:\n", module_node->name);
        }

        compile_code_to_x86(&(module_node->base.ircode), &x86_code);
        x86_code_append(&x86_code, x86_instr_new(X86_OP_ret));
        optimize_x86_code(&x86_code);
        x86_code_print(&x86_code, ofp);
        x86_code_clear(&x86_code);
    }

    fprintf(ofp, EPILOGUE_STR, data_size);
}

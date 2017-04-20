/* Compiles IRCode to x86 assembly */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ircode.h"
#include "addr.h"
#include "ast.h"
#include "x86.h"
#include "type.h"
#include "error.h"

//-lib-inclusion------------------------------------------------------------------

const char* ASM_LIB_FILES[] = {
    "data/common.asm",
    "data/print_integer.asm",
    "data/print_boolean.asm",
    "data/print_integer_array.asm",
    "data/print_boolean_array.asm",
    "data/read_integer_array.asm",
    "data/read_boolean_array.asm",
};

bool include_lib_files[] = {true, false, false, false, false, false, false};

void enable_output(valtype_t type, int size) {
    include_lib_files[0] = true;
    int index = 1;
    if(size > 0) {
        index += 2;
    }
    if(type == TYPE_BOOLEAN) {
        index++;
    }
    include_lib_files[index] = true;
}

void enable_input(valtype_t type) {
    include_lib_files[0] = true;
    int index = 5;
    if(type == TYPE_BOOLEAN) {
        index++;
    }
    include_lib_files[index] = true;
}

//--------------------------------------------------------------------------------

const char DATA_REG[] = "rbx";

#define TEMP_STR_SIZE 50
char temp_str[TEMP_STR_SIZE];

char std_regs[4][4][10];
char std_size_name[4][10];

void optimize_x86_code(X86Code* code) {
}

void compile_x86_error(const char* msg) {
    fprintf(stderr, "x86 compile error: %s", msg);
}

void addr_to_x86_arg(const AddrNode* an, char* dest, bool add_size, const char* index_reg) {
    if(an == NULL) {
        dest[0] = '\0';
        return;
    }
    switch(an->addr_type) {
        case ADDR_VAR:
        case ADDR_TEMP:
            if(add_size) {
                snprintf(dest, X86_ARG_SIZE, "%s [%s + %d]", std_size_name[an->type],
                    DATA_REG, an->offset);
            }
            else {
                snprintf(dest, X86_ARG_SIZE, "[%s + %d]", DATA_REG, an->offset);
            }
            break;
        case ADDR_CONST:
            if(an->type == TYPE_INTEGER) {
                snprintf(dest, X86_ARG_SIZE, "%d", an->imm.i);
            }
            else if(an->type == TYPE_REAL) {
                snprintf(dest, X86_ARG_SIZE, "%lf", an->imm.f);
            }
            else {
                snprintf(dest, X86_ARG_SIZE, "%c", (an->imm.b)?'1':'0');
            }
            break;
        case ADDR_ARR: {
            int size = TYPE_SIZES[an->type];
            int base_addr = an->base_addr->offset;
            if(an->index->addr_type == ADDR_CONST) {
                int i = an->index->imm.i;
                if(add_size) {
                    snprintf(dest, X86_ARG_SIZE, "%s [%s + %d]", std_size_name[an->type],
                        DATA_REG, base_addr + size * (i-1));
                }
                else {
                    snprintf(dest, X86_ARG_SIZE, "[%s + %d]", DATA_REG, base_addr + size * (i-1));
                }
            }
            else if(add_size) {
                snprintf(dest, X86_ARG_SIZE, "%s [%s + %s*%d + %d]", std_size_name[an->type],
                    DATA_REG, index_reg, size, base_addr - size);
            }
            else {
                snprintf(dest, X86_ARG_SIZE, "[%s + %s*%d + %d]", DATA_REG, index_reg,
                    size, base_addr - size);
            }
            break;
        }
    }
}

void load_index_if_needed(X86Code* ocode, const AddrNode* an, const char* index_reg) {
    if(an->addr_type == ADDR_ARR && an->index->addr_type == ADDR_VAR) {
        X86Instr* onode = x86_instr_new(X86_OP_mov);
        x86_code_append(ocode, onode);
        strcpy(onode->arg1, index_reg);
        snprintf(onode->arg2, X86_ARG_SIZE, "[%s + %d]", DATA_REG, an->index->offset);
    }
}

void op_addr_to_reg(X86Code* ocode, x86_op_t opcode, int regno, const AddrNode* an) {
    X86Instr* onode = x86_instr_new2(opcode, std_regs[regno][an->type], NULL);
    load_index_if_needed(ocode, an, "si");
    addr_to_x86_arg(an, onode->arg2, false, "rsi");
    x86_code_append(ocode, onode);
}

void op_reg_to_addr(X86Code* ocode, x86_op_t opcode, const AddrNode* an, int regno) {
    X86Instr* onode = x86_instr_new2(opcode, NULL, std_regs[regno][an->type]);
    load_index_if_needed(ocode, an, "di");
    addr_to_x86_arg(an, onode->arg1, false, "di");
    x86_code_append(ocode, onode);
}

void op_apply(X86Code* ocode, x86_op_t opcode, const AddrNode* an) {
    X86Instr* onode = x86_instr_new(opcode);
    load_index_if_needed(ocode, an, "si");
    addr_to_x86_arg(an, onode->arg1, true, "rsi");
    x86_code_append(ocode, onode);
}

bool check_arg(const AddrNode* an) {
    if(an != NULL && an->type != TYPE_INTEGER && an->type != TYPE_BOOLEAN) {
        print_error("x86_codegen", ERROR, -1, an->line, an->col, TYPE_STRS[an->type],
            "BAD_TYPE", "Bad type of argument");
        return false;
    }
    return true;
}

x86_op_t get_setcode(op_t op) {
    switch(op) {
        case OP_EQ: return X86_OP_sete;
        case OP_NE: return X86_OP_setne;
        case OP_LT: return X86_OP_setl;
        case OP_LE: return X86_OP_setle;
        case OP_GT: return X86_OP_setg;
        case OP_GE: return X86_OP_setge;
        default:
            return -1;
    }
}

x86_op_t get_opcode(op_t op) {
    switch(op) {
        case OP_PLUS: return X86_OP_add;
        case OP_MINUS: return X86_OP_sub;
        case OP_MUL: return X86_OP_imul;
        case OP_DIV: return X86_OP_idiv;
        case OP_AND: return X86_OP_and;
        case OP_OR: return X86_OP_or;
        default:
            return -1;
    }
}

void compile_instr_to_x86(const IRInstr* inode, X86Code* ocode) {
    if(!check_arg(inode->arg1)) return;
    if(!check_arg(inode->arg2)) return;
    if(!check_arg(inode->res)) return;
    switch(inode->op) {
        case OP_MOV:
            op_addr_to_reg(ocode, X86_OP_mov, 0, inode->arg1);
            op_reg_to_addr(ocode, X86_OP_mov, inode->res, 0);
            break;
        case OP_PLUS:
        case OP_MINUS:
        case OP_AND:
        case OP_OR:
            op_addr_to_reg(ocode, X86_OP_mov, 0, inode->arg1);
            op_addr_to_reg(ocode, get_opcode(inode->op), 0, inode->arg2);
            op_reg_to_addr(ocode, X86_OP_mov, inode->res, 0);
            break;
        case OP_UMINUS: {
            x86_code_append(ocode, x86_instr_new2(X86_OP_xor, "rax", "rax"));
            op_addr_to_reg(ocode, X86_OP_sub, 0, inode->arg1);
            op_reg_to_addr(ocode, X86_OP_mov, inode->res, 0);
            break;
        }
        case OP_MUL:
            op_addr_to_reg(ocode, X86_OP_mov, 0, inode->arg1);
            op_apply(ocode, X86_OP_imul, inode->arg2);
            op_reg_to_addr(ocode, X86_OP_mov, inode->res, 0);
            break;
        case OP_DIV:
            op_addr_to_reg(ocode, X86_OP_mov, 0, inode->arg1);
            x86_code_append(ocode, x86_instr_new2(X86_OP_xor, "rdx", "rdx"));
            op_apply(ocode, X86_OP_idiv, inode->arg2);
            op_reg_to_addr(ocode, X86_OP_mov, inode->res, 0);
            break;

        case OP_EQ:
        case OP_NE:
        case OP_LT:
        case OP_LE:
        case OP_GT:
        case OP_GE:
            op_addr_to_reg(ocode, X86_OP_mov, 0, inode->arg1);
            op_addr_to_reg(ocode, X86_OP_cmp, 0, inode->arg2);
            op_apply(ocode, get_setcode(inode->op), inode->res);
            break;

        case OP_OUTPUT: {
            int size = inode->arg1->size;
            enable_output(inode->arg1->type, size);
            if(size == 0) {
                op_addr_to_reg(ocode, X86_OP_mov, 2, inode->arg1);
                snprintf(temp_str, TEMP_STR_SIZE, "print_%s", TYPE_STRS[inode->arg1->type]);
                x86_code_append(ocode, x86_instr_new2(X86_OP_call, temp_str, NULL));
            }
            else {
                op_addr_to_reg(ocode, X86_OP_lea, 3, inode->arg1);
                snprintf(temp_str, TEMP_STR_SIZE, "%d", size);
                x86_code_append(ocode, x86_instr_new2(X86_OP_mov, "rdx", temp_str));
                snprintf(temp_str, TEMP_STR_SIZE, "print_%s_array", TYPE_STRS[inode->arg1->type]);
                x86_code_append(ocode, x86_instr_new2(X86_OP_call, temp_str, NULL));
            }
            break;
        }
        case OP_INPUT: {
            int size = inode->res->size;
            if(size == 0) size = 1;
            enable_input(inode->res->type);
            op_addr_to_reg(ocode, X86_OP_lea, 3, inode->res);
            snprintf(temp_str, TEMP_STR_SIZE, "%d", size);
            x86_code_append(ocode, x86_instr_new2(X86_OP_mov, "rdx", temp_str));
            snprintf(temp_str, TEMP_STR_SIZE, "read_%s_array", TYPE_STRS[inode->res->type]);
            x86_code_append(ocode, x86_instr_new2(X86_OP_call, temp_str, NULL));
            break;
        }
        default:
            break;
    }
}

void compile_code_to_x86(const IRCode* icode, X86Code* ocode) {
    IRInstr* inode = icode->first;
    for(; inode != NULL; inode = inode->next) {
        compile_instr_to_x86(inode, ocode);
    }
}

const char PROLOGUE_STR[] =
    X86_INDENT_STR "global main\n"
    "section .bss\n"
    "data:\n"
    X86_INDENT_STR "resb %d\n\n"
    "section .text\n";

void compile_program_to_x86(ProgramNode* program_node, pSD psd, FILE* ofp) {
    int data_size = psd->root->value->max_offset;

    fprintf(ofp, PROLOGUE_STR, data_size);

    strcpy(std_regs[0][TYPE_INTEGER], "ax");    // std_regs[0] must be "ax" for multiply to work
    strcpy(std_regs[0][TYPE_BOOLEAN], "al");
    strcpy(std_regs[0][TYPE_REAL], "xmm0");
    strcpy(std_regs[1][TYPE_INTEGER], "rax");
    strcpy(std_regs[1][TYPE_BOOLEAN], "rax");
    strcpy(std_regs[1][TYPE_REAL], "rax");
    strcpy(std_regs[2][TYPE_INTEGER], "cx");
    strcpy(std_regs[2][TYPE_BOOLEAN], "cl");
    strcpy(std_regs[2][TYPE_REAL], "xmm1");
    strcpy(std_regs[3][TYPE_INTEGER], "rcx");
    strcpy(std_regs[3][TYPE_BOOLEAN], "rcx");
    strcpy(std_regs[3][TYPE_REAL], "rcx");
    strcpy(std_size_name[TYPE_INTEGER], "word");
    strcpy(std_size_name[TYPE_BOOLEAN], "byte");
    strcpy(std_size_name[TYPE_REAL], "dword");

    X86Code x86_code;
    x86_code_init(&x86_code);
    ModuleNode* module_node = program_node->modules;
    for(; module_node != NULL; module_node = module_node->next) {
        if(module_node->name == NULL) {
            fprintf(ofp, "main:\n");
        }
        else {
            fprintf(ofp, "module_%s:\n", module_node->name);
        }

        x86_code_append(&x86_code, x86_instr_new2(X86_OP_mov, DATA_REG, "data"));
        x86_code_append(&x86_code, x86_instr_new2(X86_OP_xor, "rsi", "rsi"));
        x86_code_append(&x86_code, x86_instr_new2(X86_OP_xor, "rdi", "rdi"));
        compile_code_to_x86(&(module_node->base.ircode), &x86_code);
        if(module_node->name == NULL) {
            x86_code_append(&x86_code, x86_instr_new2(X86_OP_mov, "rax", "0"));
        }
        x86_code_append(&x86_code, x86_instr_new(X86_OP_ret));
        optimize_x86_code(&x86_code);
        x86_code_print(&x86_code, ofp);
        x86_code_clear(&x86_code);
    }

    // read libs and output them
    int num_libs = sizeof(include_lib_files) / sizeof(bool);
    int i;
    for(i=0; i<num_libs; ++i) {
        if(include_lib_files[i]) {
            fputc('\n', ofp);
            FILE* libfp = fopen(ASM_LIB_FILES[i], "r");
            setvbuf(libfp, NULL, _IOFBF, BUFSIZ);   // fully buffer file
            if (libfp) {
                int c;
                while ((c = fgetc(libfp)) != EOF)
                    fputc(c, ofp);
                fclose(libfp);
            }
        }
    }
}

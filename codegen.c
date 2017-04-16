#include <stdio.h>
#include "codegen.h"
#include "ast_defs.h"
#include "symbol_table.h"


quadruple * get_new_quadruple(operation_t op, addr_t arg1, addr_t arg2, addr_t res) {
	quadruple * ret = (quadruple*)malloc(sizeof(quadruple));
	memset(ret,0,sizeof(quadruple));
	ret->op = op;
	ret->arg1 = arg1;
	ret->arg2 = arg2;
	ret->res = res;
	return res;
}

int get_opcode(pAstNode ast_root) {
	switch(ast_root->base.node_type) {
		case ASTN_BOp:
			if(ast_root->op == OP_PLUS)
				return OPCODE_ADD;
			else if(ast_root->op == OP_MINUS)
				return OPCODE_SUB;
			else if(ast_root->op == OP_MUL)
				return OPCODE_MUL;
			else if(ast_root->op == OP_DIV)
				return OPCODE_DIV;
			break;
		case ASTN_UOp:
			if(ast_root->op == OP_PLUS)
				return OPCODE_PLUS;
			else if(ast_root->op == OP_MINUS)
				return OPCODE_MINUS;
			break;
	}

}

addr_t get_new_temp(){
}

void gen_intermediate_code(pSD temp_symbol_database, pSD user_symbol_database, pAstNode ast_root) {
	switch(ast_root->base.node_type) {
		case ASTN_Program:
			SD_add_scope(temp_symbol_database,ast_root);
		case ASTN_BOp:
			ast_root->arg1->base.q_list.last->next = ast_root->arg2->base.q_list->first;
			ast_root->base.q_list.first = ast_root->arg1->base.q_list.first;
			ast_root->addr = get_new_temp();
			quadruple* new = get_new_quadruple(
					get_opcode(ast_root),
					ast_root->arg1->addr,
					ast_root->arg2->addr,
					ast_root->addr);
			ast_root->arg2->base.q_list.last->next = new;
			ast_root->base.q_list.last = new;
			break;
		case ASTN_UOp:
			break;
		case ASTN_Assn:
			break;
	}
}

void code_gen_main(pSD user_symbol_database, pAstNode ast_root) {
	SD temp_symbol_database;
	SD_init(&temp_symbol_database);
	gen_intermediate_code(temp_symbol_database, user_symbol_database, ast_root);
}

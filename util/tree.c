#include "tree.h"
#include <stdio.h>
#include <stdlib.h>

TreeNode * get_new_tree_node(Symbol * symbol){
    TreeNode * new_node = (TreeNode *)malloc(sizeof(TreeNode));
    char no_memory_err[100] = "No memory!!\n";
    if(new_node == NULL){
	fprintf(stderr,"%s",no_memory_err);
    }else{
	new_node->symbol	= symbol;
	new_node->parent	= NULL;
	new_node->first_child	= NULL;
	new_node->next_sibling	= NULL;
	new_node->prev_sibling	= NULL;
	new_node->last_child	= NULL;
    }
    return new_node;
}
// Or should it be insert_node(TreeNode * parent, Token * new) ??
TreeNode * insert_node(TreeNode * parent, Symbol * symbol){
    TreeNode * new_node = get_new_tree_node(symbol);
    new_node->parent = parent;
    if(parent->first_child == NULL && parent->last_child == NULL){
	parent->first_child = new_node;
	parent->last_child  = new_node;
    }else{
	new_node->parent = parent;
	new_node->prev_sibling = parent->last_child;
	parent->last_child->next_sibling = new_node;
	parent->last_child = new_node;
    }
    return new_node;
}


/*
int main(int argc, char ** argv)
{
    
    return 0;
}
*/

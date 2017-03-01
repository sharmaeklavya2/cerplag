#include "tree.h"
#include <stdio.h>
#include <stdlib.h>

void get_empty_tree(Tree * t)
{
    t = (Tree *)malloc(sizeof(Tree));
    t->root_node    = NULL;
}

void get_new_tree_node(TreeNode * new_node, int symbol){
    new_node = (TreeNode *)malloc(sizeof(TreeNode));
    char no_memory_err[100] = "No memory!!\n";
    if(new_node == NULL){
	fprintf(stderr,"%s",no_memory_err);
    }else{
	new_node->symbol	= symbol;
	new_node->parent	= NULL;
	new_node->first_child	= NULL;
	new_node->next_sibling	= NULL;
	new_node->prev_sibling	= NULL;
    }
}
// Or should it be insert_node(TreeNode * parent, Token * new) ??
void insert_node(TreeNode * parent, TreeNode * new_node){
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
}


int main(int argc, char ** argv)
{
    
    return 0;
}

#include "tree.h"
#include <stdio.h>
#include <stdlib.h>

void destroy_value(tree_type value)
{
    if(value->dyn_lexeme) {
        free(value->lexeme);
        value->lexeme = NULL;
        value->dyn_lexeme = false;
    }
    free(value);
}

TreeNode * get_new_tree_node(tree_type value)
{
    TreeNode * new_node = (TreeNode *)malloc(sizeof(TreeNode));
    if(new_node == NULL){
        fprintf(stderr, "tree: No memory!\n");
    }else{
        new_node->value         = value;
        new_node->parent        = NULL;
        new_node->first_child   = NULL;
        new_node->next_sibling  = NULL;
        new_node->prev_sibling  = NULL;
        new_node->last_child    = NULL;
    }
    return new_node;
}

TreeNode * insert_node(TreeNode * parent, tree_type value){
    TreeNode * new_node = get_new_tree_node(value);
    new_node->parent = parent;
    if(parent->first_child == NULL && parent->last_child == NULL){
        parent->first_child = new_node;
    }else{
        new_node->prev_sibling = parent->last_child;
        parent->last_child->next_sibling = new_node;
    }
    parent->last_child = new_node;
    return new_node;
}

void destroy_tree(TreeNode* root)
{
    if(root != NULL)
    {
        TreeNode* sibling = root->next_sibling;
        TreeNode* child = root->first_child;
        destroy_value(root->value);
        free(root);
        destroy_tree(child);
        destroy_tree(sibling);
    }
}

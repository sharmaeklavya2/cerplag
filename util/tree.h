#ifndef H_TREE
#define H_TREE

#include "../token.h"
#include "../parser_defs.h"

typedef struct TreeNode
{
    Symbol * symbol;     
    struct TreeNode * parent;
    struct TreeNode * first_child;
    struct TreeNode * last_child;
    struct TreeNode * next_sibling;
    struct TreeNode * prev_sibling;
}TreeNode;

TreeNode * get_new_tree_node(Symbol*);

TreeNode * insert_node(TreeNode*, Symbol*);

#endif

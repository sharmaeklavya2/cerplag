#ifndef H_TREE
#define H_TREE

#include "../parser_defs.h"

typedef Symbol* tree_type;

typedef struct TreeNode
{
    tree_type value;
    struct TreeNode * parent;
    struct TreeNode * first_child;
    struct TreeNode * last_child;
    struct TreeNode * next_sibling;
    struct TreeNode * prev_sibling;
}TreeNode;

TreeNode * get_new_tree_node(tree_type);

TreeNode * insert_node(TreeNode*, tree_type);

#endif

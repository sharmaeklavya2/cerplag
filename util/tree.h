#ifndef H_TREE
#define H_TREE

#include "token.h"

typedef struct
{
    TreeNode * root_node;
}Tree;

typedef struct
{
    int token;
    TreeNode * parent;
    TreeNode * first_child;
    TreeNode * last_child;
    TreeNode * next_sibling;
    TreeNode * prev_sibling;
}TreeNode;

#endif

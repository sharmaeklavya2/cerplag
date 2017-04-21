#include <stdio.h>
#include <stdlib.h>

int TYPED(tree_node_count) = 0;

TYPED(tree_node)* TYPED(tree_get_node)(TYPE value)
{
    TYPED(tree_node)* new_node = malloc(sizeof(TYPED(tree_node)));
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
    TYPED(tree_node_count)++;
    return new_node;
}

TYPED(tree_node)* TYPED(tree_insert)(TYPED(tree_node)* parent, TYPE value){
    TYPED(tree_node)* new_node = TYPED(tree_get_node)(value);
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

void TYPED(tree_destroy)(TYPED(tree_node)* root)
{
    if(root != NULL)
    {
        TYPED(tree_node)* sibling = root->next_sibling;
        TYPED(tree_node)* child = root->first_child;
        TYPED(destroy)(root->value);
        free(root);
        TYPED(tree_destroy)(child);
        TYPED(tree_destroy)(sibling);
    }
}

void TYPED(tree_print_helper)(TYPED(tree_node)* root, FILE* fp, int indent, void printer(TYPE, FILE*)) {
    if(root != NULL) {
        int i;
        for(i=0; i<indent; ++i)
            fprintf(fp, "  ");
        printer(root->value, fp);
        fprintf(fp, "\n");
        TYPED(tree_print_helper)(root->first_child, fp, indent+1, printer);
        TYPED(tree_print_helper)(root->next_sibling, fp, indent, printer);
    }
}

void TYPED(tree_print)(TYPED(tree_node)* root, FILE* fp) {
    if(root == NULL)
        fprintf(fp, "null_tree\n");
    else
        TYPED(tree_print_helper)(root, fp, 0, TYPED(print));
}

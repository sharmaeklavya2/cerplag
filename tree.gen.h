typedef struct TYPED(tree_node)
{
    TYPE value;
    struct TYPED(tree_node)* parent;
    struct TYPED(tree_node)* first_child;
    struct TYPED(tree_node)* last_child;
    struct TYPED(tree_node)* next_sibling;
    struct TYPED(tree_node)* prev_sibling;
}TYPED(tree_node);

TYPED(tree_node) * TYPED(tree_get_node)(TYPE);

TYPED(tree_node) * TYPED(tree_insert)(TYPED(tree_node)*, TYPE);

void TYPED(tree_destroy)(TYPED(tree_node)* root);

typedef void TYPED(tree_printer)(const TYPED(tree_node)* root, FILE* fp);

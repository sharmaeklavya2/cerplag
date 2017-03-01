#ifndef H_STACK
#define H_STACK

#include <stdbool.h>

typedef struct StackNode{
    int symbol;
    struct StackNode * next_node;
}StackNode;

typedef struct Stack{
    StackNode * top;
    bool is_empty;
}Stack;

typedef Stack * stack_t;
typedef StackNode * stack_node_t;

#endif

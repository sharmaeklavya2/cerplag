#ifndef H_STACK
#define H_STACK

#include <stdbool.h>

typedef struct{
    StackNode * top;
    bool is_empty;
}Stack;

typedef struct{
    int symbol;
    StackNode * next_node;
}StackNode;

typedef Stack * stack_t;
typedef StackNode * stack_node_t;

#endif

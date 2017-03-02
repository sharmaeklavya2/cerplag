#include <stdio.h>

typedef struct TYPED(StackNode){
    TYPE value;
    struct TYPED(StackNode)* next;
}TYPED(StackNode);

typedef struct{
    TYPED(StackNode)* top;
    int size;
}TYPED(Stack);

TYPED(StackNode)* TYPED(get_stack_node)(TYPE value, TYPED(StackNode)* next);

void TYPED(stack_init)(TYPED(Stack) *ps);

void TYPED(stack_destroy)(TYPED(Stack) *ps);

void TYPED(stack_push)(TYPED(Stack)* ps, TYPE value);

TYPE TYPED(stack_top)(TYPED(Stack)* ps);

TYPE TYPED(stack_pop)(TYPED(Stack)* ps);

void TYPED(stack_print)(TYPED(Stack)* ps, FILE* fp);
// Prints stack top to bottom

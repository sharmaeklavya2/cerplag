#include <stdio.h>

typedef struct TYPED(stack_node){
    TYPE value;
    struct TYPED(stack_node)* next;
}TYPED(stack_node);

typedef struct{
    TYPED(stack_node)* top;
    int size;
}TYPED(stack);

TYPED(stack_node)* TYPED(get_stack_node)(TYPE value, TYPED(stack_node)* next);

void TYPED(stack_init)(TYPED(stack) *ps);

void TYPED(stack_destroy)(TYPED(stack) *ps);

void TYPED(stack_push)(TYPED(stack)* ps, TYPE value);

TYPE TYPED(stack_top)(TYPED(stack)* ps);

TYPE TYPED(stack_pop)(TYPED(stack)* ps);

void TYPED(stack_print)(TYPED(stack)* ps, FILE* fp);
// Prints stack top to bottom

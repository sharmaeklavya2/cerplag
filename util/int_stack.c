#include <stdio.h>
#include "int_stack.h"
#include "int.h"

#define TYPE int
#define TYPED(x) int_##x

//#include "../parser.h"

#include "stack.gen.c"

#undef TYPE
#undef TYPED

// Driver program to check functionning of stack
/*
int main()
{
    const int n = 5;
    int token_vals[5] = {12,2,323,52,1};
    int_stack s;
    int_stack_init(&s);
    int_stack_print(&s, stdout);
    for(int i=0; i<n; i++){
        int_stack_push(&s, token_vals[i]);
        int_stack_print(&s, stdout);
    }
    for(int i=0; i<=n; i++){
        int v = int_stack_pop(&s);
        printf("%d\n", v);
        int_stack_print(&s, stdout);
    }
    return 0;
}
*/

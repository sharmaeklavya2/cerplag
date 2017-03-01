#include <stdio.h>
#include "int_stack.h"

#define TYPE int
#define TYPED(x) int_##x

//#include "../parser.h"

void int_print(int x, FILE* fp)
{fprintf(fp, "%d", x);}
//{fprintf(fp, "%s", GS_STRS[x]);}

#include "stack.gen.c"

#undef TYPE
#undef TYPED

// Driver program to check functionning of stack
/*
int main()
{
    const int n = 5;
    int token_vals[5] = {12,2,323,52,1};
    int_Stack s;
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

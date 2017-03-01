#include <stdio.h>
#include <stdlib.h>


TYPED(StackNode)* TYPED(get_stack_node)(TYPE value, TYPED(StackNode)* next)
{
    TYPED(StackNode)* sn = malloc(sizeof(TYPED(StackNode)));
    sn->value = value;
    sn->next = next;
    return sn;
}

void TYPED(stack_init)(TYPED(Stack) *ps)
{
    ps->top = NULL;
    ps->size = 0;
}

void TYPED(stack_push)(TYPED(Stack)* ps, TYPE value)
{
    ps->top = TYPED(get_stack_node)(value, ps->top);
    (ps->size)++;
}

TYPE TYPED(stack_top)(TYPED(Stack)* ps)
{
    if(ps->size == 0)
        fprintf(stderr, "top called on empty stack\n");
    else
        return ps->top->value;
}

TYPE TYPED(stack_pop)(TYPED(Stack)* ps)
{
    if(ps->size == 0)
        fprintf(stderr, "pop called on empty stack\n");
    else
    {
        TYPED(StackNode)* tmp = ps->top;
        ps->top = tmp->next;
        TYPE v = tmp->value;
        free(tmp);
        (ps->size)--;
        return v;
    }
}

// Prints stack top to bottom
void TYPED(stack_print)(TYPED(Stack)* ps, FILE* fp)
{
    fprintf(fp, "[");
    TYPED(StackNode)* p = ps->top;
    if(p != NULL)
    {
        TYPED(print)(p->value, fp);
        p = p->next;
        while(p != NULL)
        {
            fprintf(fp, ", ");
            TYPED(print)(p->value, fp);
            p = p->next;
        }
    }
    fprintf(fp, "]\n");
}

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

void TYPED(stack_destroy)(TYPED(Stack) *ps)
{
    TYPED(StackNode) *p = ps->top, *todel = NULL;
    while(p != NULL)
    {
        todel = p;
        p = p->next;
        todel->next = NULL;
        TYPED(destroy)(todel->value);
        free(todel);
    }
    ps->top = NULL;
    ps->size = 0;
}

void TYPED(stack_push)(TYPED(Stack)* ps, TYPE value)
{
    /*
    fprintf(stderr, "Pushing: ");
    TYPED(print)(value,stderr);
    fprintf(stderr, "\n");
    */
    if(ps->size > 100){
        fprintf(stderr,"Stack size exceeded limit!\n");
        exit(1);
    }
    ps->top = TYPED(get_stack_node)(value, ps->top);
    (ps->size)++;
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

#pragma GCC diagnostic ignored "-Wreturn-type"

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
        /*
        fprintf(stderr, "Popping: ");
        TYPED(print)(v, stderr);
        fprintf(stderr, "\n");
        */
        return v;
    }
}

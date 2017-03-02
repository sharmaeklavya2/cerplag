#include <stdio.h>
#include <stdlib.h>


TYPED(stack_node)* TYPED(get_stack_node)(TYPE value, TYPED(stack_node)* next)
{
    TYPED(stack_node)* sn = malloc(sizeof(TYPED(stack_node)));
    sn->value = value;
    sn->next = next;
    return sn;
}

void TYPED(stack_init)(TYPED(stack) *ps)
{
    ps->top = NULL;
    ps->size = 0;
}

void TYPED(stack_destroy)(TYPED(stack) *ps)
{
    TYPED(stack_node) *p = ps->top, *todel = NULL;
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

void TYPED(stack_push)(TYPED(stack)* ps, TYPE value)
{
    /*
    fprintf(stderr, "Pushing: ");
    TYPED(print)(value,stderr);
    fprintf(stderr, "\n");
    */
    if(ps->size > 100){
        fprintf(stderr,"stack size exceeded limit!\n");
        exit(1);
    }
    ps->top = TYPED(get_stack_node)(value, ps->top);
    (ps->size)++;
}

// Prints stack top to bottom
void TYPED(stack_print)(TYPED(stack)* ps, FILE* fp)
{
    fprintf(fp, "[");
    TYPED(stack_node)* p = ps->top;
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

TYPE TYPED(stack_top)(TYPED(stack)* ps)
{
    if(ps->size == 0)
        fprintf(stderr, "top called on empty stack\n");
    else
        return ps->top->value;
}

TYPE TYPED(stack_pop)(TYPED(stack)* ps)
{
    if(ps->size == 0)
        fprintf(stderr, "pop called on empty stack\n");
    else
    {
        TYPED(stack_node)* tmp = ps->top;
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

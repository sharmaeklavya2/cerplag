#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"


void get_new_stack_node(stack_node_t sn, int token){
    sn = (stack_node_t)malloc(sizeof(stack_node_t));
    sn->token = token;
    sn->next_node = NULL;
}

void get_empty_stack(stack_t s){
    s = (stack_t)malloc(sizeof(Stack));
    s->is_empty = true;
    s->top	= NULL;
}

void push(stack_t s, stack_node_t new_node){
    if(s->is_empty){
	s->top = new_node;
	s->is_empty = false;
    }else{
	stack_node_t tmp = s->top;
	s->top = new_node;
	new_node->next_node = tmp;
    }
}

stack_node_t top(stack_t s){
    char stack_empty_err[100] = "Stack is empty. top() failed.\n";
    if(s->is_empty){
	fprintf(stderr,"%s",stack_empty_err);
	return NULL;
    }else{
	return s->top;
    }
}

void pop(stack_t s){
    char stack_empty_err[100] = "Stack is empty. pop() failed.\n";
    if(s->is_empty){
	fprintf(stderr,"%s",stack_empty_err);
    }else{
	stack_node_t tmp = s->top;
	s->top = tmp->next_node;
	free(tmp);
	if(s->top == NULL) s->is_empty = true;
    }
}

void print_node(stack_node_t sn){ fprintf(stdout,"%d",sn->token); }

// Prints stack top to bottom
void print_stack(stack_t s){
    char stack_empty_msg[100] = "Stack is empty.";
    if(s->is_empty){
	fprintf(stdout,"%s\n",stack_empty_msg)
    }else{
	stack_node_t tmp = s->top;
	while(tmp != NULL){
	    print_node(tmp);
	    fprintf(stdout,"->");
	    tmp = tmp->next_node;
	}
	fprintf(stdout,"NULL\n");
    }
}


// Driver program to check functionning of stack
int main(int argc, char ** argv){
    int token_vals[5] = {12,2,323,52,1}
    stack_t s;
    print_stack(s);
    for(int i=0; i<5; i++){
	stack_node_t tmp;
	get_new_stack_node(tmp,5);
	print_stack(s);
    }
    
    return 0;
}

#include <stdio.h>
#include <stdbool.h>

typedef struct CLASS {
    TYPE first, last;
    int size;
} CLASS;

void CLASSED(init)(CLASS*);
void CLASSED(copy)(CLASS* dest, CLASS* src);
void CLASSED(combine)(CLASS* node, CLASS* p1, CLASS* p2);

void CLASSED(prepend)(CLASS*, TYPE);
void CLASSED(append)(CLASS*, TYPE);
void CLASSED(remove)(CLASS*, TYPE);
void CLASSED(clear)(CLASS*);

void CLASSED(print)(CLASS* node, FILE* fp);

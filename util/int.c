#include <stdbool.h>
#include <stdio.h>

void int_destroy(int x){}

bool int_equals(int x, int y)
{return x == y;}

unsigned int_hash(int x)
{return (unsigned)x;}

void int_print(int x, FILE* fp)
{fprintf(fp, "%d", x);}

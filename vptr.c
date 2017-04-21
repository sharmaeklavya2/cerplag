#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "vptr.h"

void vptr_destroy(vptr p)
{
#ifdef LOG_MEM
    fprintf(stderr, "Called pch_destroy(%p)\n", (void*)p);
#endif
    free((void*)p);
}

bool vptr_equals(vptr p1, vptr p2)
{return p1 == p2;}

unsigned vptr_hash(vptr p)
{return (unsigned)(long long)p;}

void vptr_print(vptr p, FILE* fp)
{fprintf(fp, "%p", p);}

#include <stdbool.h>
#include <stdio.h>
#include "vptr.h"

void vptr_destroy(vptr p) {}

bool vptr_equals(vptr p1, vptr p2)
{return p1 == p2;}

unsigned vptr_hash(vptr p)
{return (unsigned)p;}

void vptr_print(vptr p, FILE* fp)
{fprintf(fp, "%p", p);}

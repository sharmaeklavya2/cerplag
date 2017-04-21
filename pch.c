#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "pch.h"

void pch_destroy(pch str)
{
#ifdef LOG_MEM
    fprintf(stderr, "Called pch_destroy(%p) (\"%s\")\n", (void*)str, str);
#endif
    free((mpch)str);
}

bool pch_equals(pch s1, pch s2)
{return strcmp(s1, s2) == 0;}

unsigned pch_hash(pch str)
{
    unsigned res = 0;
    for(int i=0; str[i] != '\0'; ++i)
        res = res * 307 + (int)(str[i]);
    //fprintf(stderr, "hash(%s) = %u\n", str, res);
    return res;
}

void pch_print(pch str, FILE* fp)
{fprintf(fp, "%s", str);}

/* Batch no. 13
2014A7PS130P: Eklavya Sharma
2014A7PS023P: Daivik Nema */
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pch.h"

unsigned pch_hash(pch str)
{
    unsigned res = 0;
    for(int i=0; str[i] != '\0'; ++i)
        res = res * 307 + (int)(str[i]);
    //fprintf(stderr, "hash(%s) = %u\n", str, res);
    return res;
}

bool pch_equals(pch s1, pch s2)
{return strcmp(s1, s2) == 0;}

void pch_destroy(pch str)
{free((mpch)str);}

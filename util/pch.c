#include <string.h>
#include <stdbool.h>

unsigned pch_hash(const char* str)
{
    unsigned res = 0;
    for(int i=0; str[i] != '\0'; ++i)
        res = res * 307 + (int)(str[i]);
    //fprintf(stderr, "hash(%s) = %u\n", str, res);
    return res;
}

bool pch_equals(const char* s1, const char* s2)
{return strcmp(s1, s2) == 0;}

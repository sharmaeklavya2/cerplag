#ifndef H_PCH
#define H_PCH

#include <stdbool.h>
#include <stdio.h>

typedef const char* pch;
typedef char* mpch;

void pch_destroy(pch);
bool pch_equals(pch, pch);
unsigned pch_hash(pch);
void pch_print(pch, FILE*);

#endif  // H_PCH

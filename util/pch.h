#ifndef H_PCH
#define H_PCH

#include <stdbool.h>

typedef const char* pch;
typedef char* mpch;

unsigned pch_hash(pch);

bool pch_equals(pch, pch);

void pch_destroy(pch);

#endif  // H_PCH

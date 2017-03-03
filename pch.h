/* Batch no. 13
2014A7PS130P: Eklavya Sharma
2014A7PS023P: Daivik Nema */
#ifndef H_PCH
#define H_PCH

#include <stdbool.h>

typedef const char* pch;
typedef char* mpch;

unsigned pch_hash(pch);

bool pch_equals(pch, pch);

void pch_destroy(pch);

#endif  // H_PCH

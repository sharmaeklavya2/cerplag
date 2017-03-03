/* Batch no. 13
2014A7PS130P: Eklavya Sharma
2014A7PS023P: Daivik Nema */
#ifndef H_BITSET
#define H_BITSET

#include <stdbool.h>

typedef unsigned long long bitset_t;
typedef int elem_t;

inline static bitset_t bitset_add(bitset_t s, elem_t a)
{return s | (1ull << a);}

inline static bitset_t bitset_remove(bitset_t s, elem_t a)
{return s & ~(1ull << a);}

inline static bool bitset_has(bitset_t s, elem_t a)
{return ((s >> a) & 1ull);}

inline static bitset_t bitset_singleton(elem_t a)
{return 1ull << a;}

#endif  // H_BITSET

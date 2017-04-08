#ifndef H_VPTR
#define H_VPTR

#include <stdbool.h>

typedef const void* vptr;

unsigned vptr_hash(vptr);

bool vptr_equals(vptr, vptr);

void vptr_destroy(vptr);

#endif  // H_PTR

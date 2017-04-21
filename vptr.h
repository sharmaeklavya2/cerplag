#ifndef H_VPTR
#define H_VPTR

#include <stdbool.h>
#include <stdio.h>

typedef const void* vptr;

void vptr_destroy(vptr);
bool vptr_equals(vptr, vptr);
unsigned vptr_hash(vptr);
void vptr_print(vptr, FILE*);

#endif  // H_PTR

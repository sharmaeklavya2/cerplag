#ifndef H_INT
#define H_INT

#include <stdbool.h>
#include <stdio.h>

void int_destroy(int);
bool int_equals(int, int);
unsigned int_hash(int);
void int_print(int, FILE*);

#endif  // H_INT

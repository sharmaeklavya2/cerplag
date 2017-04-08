#include <stdbool.h>
#include "vptr.h"

unsigned vptr_hash(vptr p)
{return (unsigned)p;}

bool vptr_equals(vptr p1, vptr p2)
{return p1 == p2;}

void vptr_destroy(vptr p) {}

#include "vptr_int_hmap.h"

#define KTYPE vptr
#define VTYPE int

#define KTYPED(x) vptr_##x
#define VTYPED(x) int_##x
#define ITYPED(x) vptr_int_##x

#include "hmap.gen.c"

#undef KTYPE
#undef VTYPE
#undef KTYPED
#undef VTYPED
#undef ITYPED

#ifndef H_VPTR_INT_HMAP
#define H_VPTR_INT_HMAP

#define KTYPE vptr
#define VTYPE int

#define KTYPED(x) vptr_##x
#define VTYPED(x) int_##x
#define ITYPED(x) vptr_int_##x

#include "vptr.h"
#include "hmap.gen.h"

#undef KTYPE
#undef VTYPE
#undef KTYPED
#undef VTYPED
#undef ITYPED

#endif  // H_VPTR_INT_HMAP

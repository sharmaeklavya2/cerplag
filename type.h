#ifndef H_TYPE
#define H_TYPE

typedef enum {TYPE_ERROR, TYPE_INTEGER, TYPE_REAL, TYPE_BOOLEAN} valtype_t;
#define TYPE_STRS_LIST {"error", "integer", "real", "boolean"}
#define TYPE_SIZES_LIST {0, 2, 4, 1}

extern char* TYPE_STRS[];
extern int TYPE_SIZES[];
extern int TYPE_ALIGNS[];

void get_type_str(char* str, valtype_t type, int size);
int get_type_width(valtype_t type, int size);
int get_aligned_offset(int offset, valtype_t type);

#endif  // H_TYPE

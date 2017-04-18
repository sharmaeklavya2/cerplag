#include "type.h"
#include <stdio.h>

char* TYPE_STRS[] = TYPE_STRS_LIST;
int TYPE_SIZES[] = TYPE_SIZES_LIST;
int TYPE_ALIGNS[] = TYPE_SIZES_LIST;

int get_type_width(valtype_t type, int size) {
    if(size == 0) {
        return TYPE_SIZES[type];
    }
    else {
        return TYPE_SIZES[type] * size;
    }
}

int get_aligned_offset(int offset, valtype_t type) {
    int align_size = TYPE_ALIGNS[type];
    int t = offset % align_size;
    if(t != 0)
        offset += (align_size - t);
    return offset;
}

void get_type_str(char* str, valtype_t type, int size) {
    if(size == 0)
        sprintf(str, "%s", TYPE_STRS[type]);
    else
        sprintf(str, "%s[%d]", TYPE_STRS[type], size);
}

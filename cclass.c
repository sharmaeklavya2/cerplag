#include <stdbool.h>
#include "cclass.h"

static cclass_t pc_cclass[256];

void precompute_cclass()
{
    int i;
    for(i=0; i < (1 << sizeof(char)); ++i)
        pc_cclass[i] = C_REST;

    for(i='a'; i<='z'; ++i)
        pc_cclass[i] = C_ALPHA;
    for(i='A'; i<='Z'; ++i)
        pc_cclass[i] = C_ALPHA;
    pc_cclass['_'] = C_UNSC;

    pc_cclass['e'] = C_E;
    pc_cclass['E'] = C_E;

    for(i='0'; i<='9'; ++i)
        pc_cclass[i] = C_DIG;

    pc_cclass[' '] = C_WS;
    pc_cclass['\t'] = C_WS;
    pc_cclass['\n'] = C_WS;
    pc_cclass['\0'] = C_EOF;
    
    pc_cclass['('] = C_SCO;
    pc_cclass[')'] = C_SCO;
    pc_cclass['['] = C_SCO;
    pc_cclass[']'] = C_SCO;
    pc_cclass[','] = C_SCO;
    pc_cclass[';'] = C_SCO;
    pc_cclass['/'] = C_SCO;
    
    pc_cclass['+'] = C_PM;
    pc_cclass['-'] = C_PM;
    pc_cclass['='] = C_EQUALS;
    pc_cclass['!'] = C_EXCL;
    pc_cclass['.'] = C_DOT;
    pc_cclass['*'] = C_STAR;
    pc_cclass[':'] = C_COLON;
    pc_cclass['<'] = C_LT;
    pc_cclass['>'] = C_GT;
}

cclass_t get_cclass(char ch)
{
    if(ch == '\0')
        return C_EOF;
    else if(ch < 0)
        return C_REST;
    else
        return pc_cclass[(int)ch];
}

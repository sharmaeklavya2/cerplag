#ifndef H_TOKEN
#define H_TOKEN

#define LEXEME_BUFSIZE 30

#define X(a, b) T_##a,
typedef enum
{
#include "data/tok.xmac"
    T_LAST
}tok_t;
#undef X

#define NUM_TOKENS T_LAST

typedef union
{int i; double f;}
i_or_f_t;

typedef struct
{
    int line, col;
    char* lexeme;
    int size;
    int tid;
    i_or_f_t num;
}Token;

#endif  // H_TOKEN

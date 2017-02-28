#ifndef H_TOKEN
#define H_TOKEN

#define LEXEME_BUFSIZE 30

#define X(a) a,
typedef enum
{
#include "data/tok.xmac"
    T_LAST
}tok_t;
#undef X

#define NUM_TOKENS T_LAST

typedef struct
{
    int line, col;
    char* lexeme;
    int size;
    tok_t tid;
}Token;

#endif  // H_TOKEN

#ifndef H_TOKEN
#define H_TOKEN

#include <stdio.h>
#include <stdbool.h>

#define LEXEME_BUFSIZE 30

#define X(a, b) T_##a,
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
    bool dyn_lexeme;
    int size;
    int tid;
    union {
        int i;
        float f;
    };
}Token;

typedef void token_printer(const Token*, FILE*);

#endif  // H_TOKEN

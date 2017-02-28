#ifndef H_TOKEN
#define H_TOKEN

#define LEXEME_BUFSIZE 30

typedef enum
{
#include "data/tok.enum"
    T_LAST
}tok_t;

#define NUM_TOKENS T_LAST

typedef struct
{
    int line, col;
    char* lexeme;
    int size;
    tok_t tid;
}Token;

#endif  // H_TOKEN

#ifndef H_TOKEN
#define H_TOKEN

#define LEXEME_BUFSIZE 50

typedef enum
{
    T_NUM = -3,
    T_BAD_SYMBOL = -2,
    T_ERR = -1,
    T_EOF = 0,
}tok_t;

typedef struct
{
    int line, col;
    char lexeme[LEXEME_BUFSIZE];
    tok_t tid;
}Token;

#endif  // H_TOKEN

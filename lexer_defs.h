#ifndef H_LEXER_DEFS
#define H_LEXER_DEFS

// Enumerations ----------------------------------------------------------------

#define X(a) a,
typedef enum
{
#include "cclass.xmac"
    C_LAST
}cclass_t;
#undef X

#define NUM_CCLASSES C_LAST

#define X(a) a,
typedef enum
{
#include "state.xmac"
    S_LAST
}state_t;
#undef X

#define NUM_STATES S_LAST

#define X(a) a,
typedef enum
{
#include "action.xmac"
    A_LAST
}action_t;
#undef X

#define NUM_ACTIONS A_LAST

#define X(a, b, c) a,
typedef enum
{
#include "lerr.xmac"
    LERR_LAST
}lerr_t;    // lexer error
#undef X

#define NUM_LERRS LERR_LAST

typedef union
{
    tok_t tok;
    lerr_t lerr;
}tok_or_err_t;

// Extern variables -----------------------------------------------------------

extern char* TOK_STRS2[];

// Data structures ------------------------------------------------------------

typedef struct
{
    state_t s;
    int line, col;
    char* lexeme;
    int size;
    bool trunc;
    int error_count;
}Dfa;

#endif  // H_LEXER_DEFS

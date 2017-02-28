#ifndef H_LEXER_DEFS
#define H_LEXER_DEFS

// Enumerations ----------------------------------------------------------------

#define X(a) a,
typedef enum
{
#include "data/cclass.xmac"
    C_LAST
}cclass_t;
#undef X

#define NUM_CCLASSES C_LAST

#define X(a) a,
typedef enum
{
#include "data/state.xmac"
    S_LAST
}state_t;
#undef X

#define NUM_STATES S_LAST

#define X(a) a,
typedef enum
{
#include "data/action.xmac"
    A_LAST
}action_t;
#undef X

#define NUM_ACTIONS A_LAST

#define X(a, b, c) a,
typedef enum
{
#include "data/lerr.xmac"
    LERR_LAST
}lerr_t;    // lexer error
#undef X

#define NUM_LERRS LERR_LAST

typedef union
{
    tok_t tok;
    lerr_t lerr;
}tok_or_err_t;

// Data structures ------------------------------------------------------------

typedef struct
{
    state_t s;
    int line, col;
    char* lexeme;
    int size;
    bool trunc;
}Dfa;

#endif  // H_LEXER_DEFS

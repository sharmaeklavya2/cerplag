#ifndef H_LEXER_DEFS
#define H_LEXER_DEFS

// Enumerations ----------------------------------------------------------------

typedef enum
{
#include "data/cclass.enum"
    C_LAST
}cclass_t;

#define NUM_CCLASSES C_LAST

typedef enum
{
#include "data/state.enum"
    S_LAST
}state_t;

#define NUM_STATES S_LAST

typedef enum
{
#include "data/action.enum"
    A_LAST
}action_t;

#define NUM_ACTIONS A_LAST

#define X_LERR(a, b, c) a,
typedef enum
{
#include "data/lerr.xmac"
    LERR_LAST
}lerr_t;    // lexer error
#undef X_LERR

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

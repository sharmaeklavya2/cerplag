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

typedef enum
{
    LERR_NONE,
    LERR_OTH,       // Other lexer error
    LERR_BAD_SYM,   // Bad symbol
    LERR_NUM,       // Saw letter after number
    LERR_UPAT,      // Invalid pattern
    LERR_LONG_ID,   // Identifier is too long
    LERR_LONG_TOK,  // Identifier is too long
    LERR_LAST
}lerr_t;    // lexer error

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

#ifndef H_LEXER
#define H_LEXER

#include <stdio.h>
#include <stdbool.h>
#include "token.h"
#include "lexer_defs.h"

void init_dfa(Dfa*);
void init_token(Token*);

void precompute_lexer(bool debug);
lerr_t get_token(FILE*, Dfa*, Token*, bool debug);
int lexer_main(FILE* in, FILE* out, int verbosity);

#endif  // H_LEXER

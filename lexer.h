#ifndef H_LEXER
#define H_LEXER

#include <stdio.h>
#include <stdbool.h>
#include "token.h"
#include "lexerDefs.h"

void init_dfa(Dfa*);
void init_token(Token*);

void init_lexer();
void destroy_lexer();
void get_token(FILE*, Dfa*, Token*, bool debug);
int lexer_main(FILE* in, FILE* out, int verbosity, token_printer tp);
void print_source_without_comments(FILE* ifp, FILE* ofp);

token_printer print_token, print_token_sub;

#endif  // H_LEXER

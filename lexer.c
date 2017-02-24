#include <stdio.h>
#include <stdbool.h>
#include "token.h"
#include "cclass.h"

#define INPUT_BUFSIZE 10
#define ERR_DETAILS_BUFSIZE 50

char ERRORS1[4][20] = {
    "",
    "ERR",
    "BAD_SYMBOL",
    "NUM"
};

char ERRORS2[4][200] = {
    "",
    "Unknown lexer error",
    "This symbol is not allowed in this language",
    "An identifier can't start with a digit"
};

static char input_buffer[INPUT_BUFSIZE];
static char* input_ptr = NULL;

char get_character(FILE* fp)
{
    if(input_ptr == NULL || *input_ptr == '\0')
    {
        input_ptr = fgets(input_buffer, INPUT_BUFSIZE, fp);
        if(input_ptr == NULL)
            return '\0';
    }
    return *(input_ptr++);
}

#define NUM_STATES 25

//static char dfa[NUM_STATES][NUM_CCLASS];

int get_token(FILE* fp, int state, Token* ptok, char* err_details)
{
    //char ch = get_character(fp);
    //cclass_t cclass = get_cclass(ch);
    
    return 0;
}
    
int lexer_main(int argc, char* argv[])
{
    char usage[] = "usage: %s filename\n";
    if(argc != 2)
    {
        fprintf(stderr, usage, argv[0]);
        return 1;
    }

    FILE* fp = fopen(argv[1], "r");
    if(fp == NULL)
    {
        perror("lexer");
        return 2;
    }

    precompute_cclass();

    Token tok;
    char err_details[ERR_DETAILS_BUFSIZE];
    bool got_error = false;
    int state = 0, state2;
    do
    {
        state2 = get_token(fp, state, &tok, err_details);
        if(tok.tid < 0)
        {
            got_error = true;
            fprintf(stderr, "%d, %d: %s\n%s\n%s\n\n", tok.line, tok.col, ERRORS1[-tok.tid], ERRORS2[-tok.tid], err_details);
        }
        else
        {
            printf("%2d %s\n", tok.tid, tok.lexeme);
        }
        state = state2;
    }
    while(tok.tid != 0);

    if(got_error)
        return 1;
    else
        return 0;
}

int main(int argc, char* argv[])
{return lexer_main(argc, argv);}

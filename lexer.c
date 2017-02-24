#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include"token.h"

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

typedef enum
{
    C_REST = 0,     // restricted
    C_ALPHA = 1,    // alphabet
    C_E = 2,        // e
    C_UNSC = 1,     // underscore
    C_DIG = 3,      // digits
    C_WS = 4,       // whitespace
    C_SCO = 5,      // single-character operator
    C_PM = 6,       // '+' or '-'
    C_EQUALS = 7,   // '='
    C_EXCL = 8,     // '!'
    C_DOT = 9,      // '.'
    C_STAR = 10,    // '*'
    C_COLON = 11,   // ':'
    C_LT = 12,      // '<'
    C_GT = 13,      // '>'
    C_EOF = 14      // EOF (end of file)
}cclass_t;

#define NUM_CCLASS 15

static cclass_t pc_cclass[256];

void precompute_char_class()
{
    int i;
    for(i=0; i<256; ++i)
        pc_cclass[i] = C_REST;

    for(i='a'; i<='z'; ++i)
        pc_cclass[i] = C_ALPHA;
    for(i='A'; i<='Z'; ++i)
        pc_cclass[i] = C_ALPHA;
    pc_cclass['_'] = C_UNSC;

    pc_cclass['e'] = C_E;
    pc_cclass['E'] = C_E;

    for(i='0'; i<='9'; ++i)
        pc_cclass[i] = C_DIG;

    pc_cclass[' '] = C_WS;
    pc_cclass['\t'] = C_WS;
    pc_cclass['\n'] = C_WS;
    
    pc_cclass['('] = C_SCO;
    pc_cclass[')'] = C_SCO;
    pc_cclass['['] = C_SCO;
    pc_cclass[']'] = C_SCO;
    pc_cclass[','] = C_SCO;
    pc_cclass[';'] = C_SCO;
    pc_cclass['/'] = C_SCO;
    
    pc_cclass['+'] = C_PM;
    pc_cclass['-'] = C_PM;
    pc_cclass['='] = C_EQUALS;
    pc_cclass['!'] = C_EXCL;
    pc_cclass['.'] = C_DOT;
    pc_cclass['*'] = C_STAR;
    pc_cclass[':'] = C_COLON;
    pc_cclass['<'] = C_LT;
    pc_cclass['>'] = C_GT;
}

cclass_t get_cclass(char ch)
{
    if(ch == EOF)
        return C_EOF;
    else if(ch < 0)
        return C_REST;
    else
        return pc_cclass[(int)ch];
}

static char input_buffer[INPUT_BUFSIZE];
static char* input_ptr = NULL;

char get_character(FILE* fp)
{
    if(input_ptr == NULL || *input_ptr == '\0')
    {
        input_ptr = fgets(input_buffer, INPUT_BUFSIZE, fp);
        if(input_ptr == NULL)
            return EOF;
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

    precompute_char_class();

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

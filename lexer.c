#include<stdio.h>
#include<string.h>
#include<stdbool.h>

#define INPUT_BUFSIZE 10
#define LEXEME_BUFSIZE 50
#define ERR_DETAILS_BUFSIZE 50

char lex_buffer[INPUT_BUFSIZE];
char* lex_ptr = NULL;
char char_class[256];

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
    T_NUM = -3,
    T_BAD_SYMBOL = -2,
    T_ERR = -1,
    T_EOF = 0,
}tok_t;

typedef enum
{
    C_REST = 0,
    C_ALPHA = 1,
    C_E = 2,
    C_DIG = 3,
    C_WS = 4,
    C_SCO = 5,
    C_PM = 6,
    C_EQUALS = 7,
    C_EXCL = 8,
    C_DOT = 9,
    C_STAR = 10,
    C_COLON = 11,
    C_LT = 12,
    C_GT = 13
}cclass_t;

void precompute_char_class()
{
    int i;
    for(i=0; i<256; ++i)
        char_class[i] = C_REST;

    for(i='a'; i<='z'; ++i)
        char_class[i] = C_ALPHA;
    for(i='A'; i<='Z'; ++i)
        char_class[i] = C_ALPHA;
    char_class['_'] = C_ALPHA;

    char_class['e'] = C_E;
    char_class['E'] = C_E;

    for(i='0'; i<='9'; ++i)
        char_class[i] = C_DIG;

    char_class[' '] = C_WS;
    char_class['\t'] = C_WS;
    char_class['\n'] = C_WS;
    
    char_class['('] = C_SCO;
    char_class[')'] = C_SCO;
    char_class['['] = C_SCO;
    char_class[']'] = C_SCO;
    char_class[','] = C_SCO;
    char_class[';'] = C_SCO;
    char_class['/'] = C_SCO;
    
    char_class['+'] = C_PM;
    char_class['-'] = C_PM;
    char_class['='] = C_EQUALS;
    char_class['!'] = C_EXCL;
    char_class['.'] = C_DOT;
    char_class['*'] = C_STAR;
    char_class[':'] = C_COLON;
    char_class['<'] = C_LT;
    char_class['>'] = C_GT;
}

char get_character(FILE* fp)
{
    if(lex_ptr == NULL || *lex_ptr == '\0')
    {
        lex_ptr = fgets(lex_buffer, INPUT_BUFSIZE, fp);
        if(lex_ptr == NULL)
            return EOF;
    }
    return *(lex_ptr++);
}

tok_t get_token(FILE* fp, char* lexeme, int maxlen, int* pline, char* err_details)
{
    
    return T_EOF;
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

    tok_t tok;
    char lexeme[LEXEME_BUFSIZE];
    char err_details[ERR_DETAILS_BUFSIZE];
    int line = 0;
    bool got_error = false;
    while(true)
    {
        tok = get_token(fp, lexeme, LEXEME_BUFSIZE, &line, err_details);
        if(tok < 0)
        {
            got_error = true;
            break;
        }
        else if(tok == 0)
            break;
            
    }

    if(got_error)
    {
        fprintf(stderr, "%d: %s\n%s\n%s\n\n", line, ERRORS1[-tok], ERRORS2[-tok], err_details);
        return 1;
    }
    else
        return 0;
}

int main(int argc, char* argv[])
{return lexer_main(argc, argv);}

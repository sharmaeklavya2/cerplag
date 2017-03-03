#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "lexer_defs.h"
#include "assert.h"
#include "util/pch_int_hmap.h"

#define INPUT_BUFSIZE 100
#define OUTPUT_BUFSIZE 100
#define ERR_DETAILS_BUFSIZE 100

// Global variables ------------------------------------------------------------

#define X(a, b, c) b,
const char* ERRORS1[] = {
#include "data/lerr.xmac"
};  // Error codes
#undef X

#define X(a, b, c) c,
const char* ERRORS2[] = {
#include "data/lerr.xmac"
};  // Error info
#undef X

#define X(a, b) "T_"#a,
char* TOK_STRS[] = {
#include "data/tok.xmac"
};
#undef X

#define X(a) #a,
char* CCLASS_STRS[] = {
#include "data/cclass.xmac"
};
char* ACTION_STRS[] = {
#include "data/action.xmac"
};
char* STATE_STRS[] = {
#include "data/state.xmac"
};
#undef X

static char input_buffer[INPUT_BUFSIZE];
static char* input_ptr = NULL;  // pointer in input buffer

static char output_buffer[OUTPUT_BUFSIZE];
static int output_ptr = -1;  // pointer in output buffer

static char buff1[LEXEME_BUFSIZE], buff2[LEXEME_BUFSIZE];

static cclass_t pc_cclass[128]; // pre-computed character class

static bool is_final[NUM_STATES];

static state_t state_table[NUM_STATES][NUM_CCLASSES];
static action_t action_table[NUM_STATES][NUM_CCLASSES];

static pch_int_hmap keyword_hmap;

// Precompute stuff ------------------------------------------------------------

void precompute_cclass(cclass_t* cclass)
// find character class of each character and store it in cclass
{
    int i;
    for(i=0; i < (1 << sizeof(char)); ++i)
        cclass[i] = C_REST;

    for(i='a'; i<='z'; ++i) cclass[i] = C_ALPHA;
    for(i='A'; i<='Z'; ++i) cclass[i] = C_ALPHA;
    cclass['e'] = C_E; cclass['E'] = C_E;
    cclass['_'] = C_UNSC;

    for(i='0'; i<='9'; ++i) cclass[i] = C_DIG;

    cclass[' '] = C_WS;  cclass['\t'] = C_WS; cclass['\r'] = C_WS;
    cclass['\0'] = C_EOF; cclass['\n'] = C_NL;

    cclass['('] = C_SCO; cclass[')'] = C_SCO;
    cclass['['] = C_SCO; cclass[']'] = C_SCO;
    cclass[','] = C_SCO; cclass[';'] = C_SCO;
    cclass['/'] = C_SCO;

    cclass['+'] = C_PM; cclass['-'] = C_PM;
    cclass['='] = C_EQ; cclass['!'] = C_EXCL;
    cclass['<'] = C_LT; cclass['>'] = C_GT;
    cclass['.'] = C_DOT;
    cclass['*'] = C_STAR;
    cclass[':'] = C_COLON;
}

void precompute_final_states(bool* is_final)
{
    static state_t final_states[] = {S_ID, S_I1, S_R1, S_R2, S_STAR, S_DOT2, S_COLON,
        S_LT, S_LT2, S_LT3, S_GT, S_GT2, S_GT3};

    int i, num_final_states = sizeof(final_states) / sizeof(state_t);
    for(i=0; i < NUM_STATES; ++i)
        is_final[i] = false;
    for(i=0; i < num_final_states; ++i)
        is_final[final_states[i]] = true;
}

inline static void add_edge(state_t s, cclass_t c, state_t s2, action_t a)
{
    state_table[s][c] = s2;
    action_table[s][c] = a;
}

void precompute_dfa()
{
    state_t s;
    cclass_t c;

    for(s=0; s < NUM_STATES; ++s)
        for(c=0; c < NUM_CCLASSES; ++c)
        {
            state_table[s][c] = S_START;
            if(is_final[s])
                action_table[s][c] = A_TOKS;
            else
                action_table[s][c] = A_ERR;
        }

    // whitespace and restricted chars

    for(c=0; c < NUM_CCLASSES; ++c)
        add_edge(S_START, c, S_START, A_TOKF);
    add_edge(S_START, C_WS, S_START, A_IGN);
    add_edge(S_START, C_NL, S_START, A_NL);

    // identifiers

    add_edge(S_START, C_ALPHA, S_ID, A_ADD);
    add_edge(S_START, C_E, S_ID, A_ADD);
    add_edge(S_START, C_UNSC, S_ID, A_ADD);
    // undescrore isn't allowed as the first character in an identifier
    // but we'll handle it later
    add_edge(S_ID, C_ALPHA, S_ID, A_ADD);
    add_edge(S_ID, C_E, S_ID, A_ADD);
    add_edge(S_ID, C_DIG, S_ID, A_ADD);
    add_edge(S_ID, C_UNSC, S_ID, A_ADD);

    // numbers

    add_edge(S_START, C_DIG, S_I1, A_ADD);

    add_edge(S_I1, C_DIG, S_I1, A_ADD);
    add_edge(S_I1, C_DOT, S_I2, A_ADD);
    add_edge(S_I1, C_E, S_E, A_ADD);

    add_edge(S_I2, C_DOT, S_DOT2, A_TOKN);
    add_edge(S_I2, C_DIG, S_R1, A_ADD);
    add_edge(S_I2, C_E, S_E, A_ADD);

    add_edge(S_R1, C_DIG, S_R1, A_ADD);
    add_edge(S_R1, C_E, S_E, A_ADD);

    add_edge(S_E, C_PM, S_E2, A_ADD);
    add_edge(S_E, C_DIG, S_R2, A_ADD);
    add_edge(S_E2, C_DIG, S_R2, A_ADD);

    add_edge(S_R2, C_DIG, S_R2, A_ADD);

    // multiplication and comments

    add_edge(S_START, C_STAR, S_STAR, A_ADD);
    add_edge(S_STAR, C_STAR, S_STAR2, A_CLR);
    for(c=0; c < NUM_CCLASSES; ++c)
        add_edge(S_STAR2, c, S_STAR2, A_IGN);
    add_edge(S_STAR2, C_NL, S_STAR2, A_NL);
    add_edge(S_STAR2, C_STAR, S_STAR3, A_IGN);
    for(c=0; c < NUM_CCLASSES; ++c)
        add_edge(S_STAR3, c, S_STAR3, A_IGN);
    add_edge(S_STAR3, C_NL, S_STAR3, A_NL);
    add_edge(S_STAR3, C_STAR, S_START, A_IGN);

    // operators

    add_edge(S_START, C_SCO, S_START, A_TOKF);
    add_edge(S_START, C_PM, S_START, A_TOKF);

    add_edge(S_START, C_EQ, S_EQ, A_ADD);
    add_edge(S_EQ, C_EQ, S_START, A_TOKF);

    add_edge(S_START, C_EXCL, S_EXCL, A_ADD);
    add_edge(S_EXCL, C_EQ, S_START, A_TOKF);

    add_edge(S_START, C_DOT, S_DOT, A_ADD);
    add_edge(S_DOT, C_DOT, S_DOT2, A_ADD);

    add_edge(S_START, C_COLON, S_COLON, A_ADD);
    add_edge(S_COLON, C_EQ, S_START, A_TOKF);

    add_edge(S_START, C_LT, S_LT, A_ADD);
    add_edge(S_LT, C_EQ, S_START, A_TOKF);
    add_edge(S_LT, C_LT, S_LT2, A_ADD);
    add_edge(S_LT2, C_LT, S_LT3, A_ADD);

    add_edge(S_START, C_GT, S_GT, A_ADD);
    add_edge(S_GT, C_EQ, S_START, A_TOKF);
    add_edge(S_GT, C_GT, S_GT2, A_ADD);
    add_edge(S_GT2, C_GT, S_GT3, A_ADD);
}

void precompute_keyword_hmap()
{
    pch_int_hmap_init(&keyword_hmap, 100);
#define X(t, k) pch_int_hmap_update(&keyword_hmap, k, T_##t);
#include "data/keywords.xmac"
#undef X
}

void init_lexer()
{
    precompute_cclass(pc_cclass);
    precompute_final_states(is_final);
    precompute_dfa();
    precompute_keyword_hmap();
}

void destroy_lexer()
{
    pch_int_hmap_destroy(&keyword_hmap);
}

// Utility functions -----------------------------------------------------------

cclass_t get_cclass(char ch)
{
    if(ch == '\0')
        return C_EOF;
    else if(ch < 0 || ch > 127)
        return C_REST;
    else
        return pc_cclass[(int)ch];
}

char get_character(FILE* fp)
{
    char ch;
    //static char fmt_str[] = "input: \'%c\' (%d)\n";
    if(input_ptr == NULL || *input_ptr == '\0')
    {
        int read = fread(input_buffer, sizeof(char), INPUT_BUFSIZE-1, fp);
        input_buffer[read] = '\0';
        input_ptr = input_buffer;
        if(read == 0)
        {
            //fprintf(stderr, fmt_str, '\0', 0);
            return '\0';
        }
    }
    ch = *(input_ptr++);
    //fprintf(stderr, fmt_str, ch, (int)ch);
    return ch;
}

void put_character(FILE * fp, char ch)
{
    if(output_ptr == OUTPUT_BUFSIZE-1)
    {
        fwrite(output_buffer, sizeof(char), OUTPUT_BUFSIZE, fp);
		output_ptr = -1;
		//output_buffer[OUTPUT_BUFFER] = '\0';
		memset(output_buffer, '\0', OUTPUT_BUFSIZE);
    }
	output_ptr ++;
	output_buffer[output_ptr] = ch;
	if(ch == '\0')
        fwrite(output_buffer, sizeof(char), output_ptr+1, fp);
}

tok_t predict_token_from_char(char ch)
{
    switch(ch)
    {
    case '+': return T_PLUS;
    case '-': return T_MINUS;
    case '(': return T_BO;
    case ')': return T_BC;
    case '[': return T_SQBO;
    case ']': return T_SQBC;
    case ',': return T_COMMA;
    case ';': return T_SEMICOL;
    case '/': return T_DIV;

    case '=': return T_EQ;
    case '!': return T_NE;
    case ':': return T_ASSIGNOP;
    case '<': return T_LE;
    case '>': return T_GE;
    default: return T_ERR;
    }
}

tok_t predict_token_from_state(state_t s)
{
    switch(s)
    {
    case S_ID: return T_ID;
    case S_I1: return T_NUM;
    case S_R1:
    case S_R2: return T_RNUM;

    case S_STAR: return T_MUL;
    case S_DOT2: return T_RANGEOP;
    case S_COLON: return T_COLON;

    case S_LT: return T_LT;
    case S_LT2: return T_DEF;
    case S_LT3: return T_DRIVERDEF;
    case S_GT: return T_GT;
    case S_GT2: return T_ENDDEF;
    case S_GT3: return T_DRIVERENDDEF;

    default: return T_ERR;
    }
}

void print_lex_error(lerr_t lerr, Dfa* pdfa, const Token* ptok)
{
    if(lerr != LERR_NONE)
    {
        (pdfa->error_count)++;
        fprintf(stderr, "lex_error_%d: line %2d, col %2d: \'%s\' \n%s: %s\n\n", lerr,
            ptok->line, ptok->col, ptok->lexeme, ERRORS1[lerr], ERRORS2[lerr]);
    }
}

// DFA implementation ----------------------------------------------------------

void swap_buffers(Dfa* pdfa, Token* ptok)
{
    char* p = pdfa->lexeme;
    pdfa->lexeme = ptok->lexeme;
    ptok->lexeme = p;
    int s = pdfa->size;
    pdfa->size = ptok->size;
    ptok->size = s;
}

void execute_action(action_t a, char ch, Dfa* pdfa, Token* ptok)
{
    lerr_t lerr = LERR_NONE;
    switch(a)
    {
    case A_ADD:
        pdfa->lexeme[pdfa->size] = ch;
        (pdfa->size)++;
        if(pdfa->size >= LEXEME_BUFSIZE)
        {
            (pdfa->size)--;
            pdfa->trunc = true;
        }
        pdfa->lexeme[pdfa->size] = '\0';
        break;
    case A_IGN:
        break;
    case A_NL:
        pdfa->line++;
        pdfa->col = 0;
        break;
    case A_CLR:
        pdfa->lexeme[0] = '\0';
        pdfa->size = 0;
        break;
    case A_ERR:
        if(get_cclass(ch) == C_REST)
            lerr = LERR_BAD_SYM;
        else if(pdfa->s == S_I1)
            lerr = LERR_NUM;
        else
            lerr = LERR_UPAT;
        // break statement intentionally left out
    case A_TOKS:
    case A_TOKF:
    case A_TOKN:
        if(a == A_TOKF)
        {
            pdfa->lexeme[(pdfa->size)++] = ch;
            pdfa->lexeme[pdfa->size] = '\0';
        }
        else if(a == A_TOKN)
            pdfa->lexeme[--(pdfa->size)] = '\0';
        else
        {
            input_ptr--;
            pdfa->col--;
        }
        swap_buffers(pdfa, ptok);

        ptok->line = pdfa->line;
        ptok->col = pdfa->col - ptok->size + 1;

        if(pdfa->trunc)
        {
            ptok->tid = T_ERR;
            lerr = LERR_LONG_TOK;
            pdfa->trunc = false;
        }
        else if(a == A_TOKS)
        {
            ptok->tid = predict_token_from_state(pdfa->s);
            if(ptok->tid == T_ERR)
                lerr = LERR_OTH;
        }
        else if(a == A_TOKF)
        {
            ptok->tid = predict_token_from_char(ptok->lexeme[0]);
            if(get_cclass(ch) == C_REST)
                lerr = LERR_BAD_SYM;
            else if(ptok->tid == T_ERR)
                lerr = LERR_UPAT;
        }
        else if(a == A_TOKN)
            ptok->tid = T_NUM;
        else
            ptok->tid = T_ERR;

        if(a == A_TOKN)
        {
            pdfa->lexeme[0] = '.'; pdfa->lexeme[1] = '.'; pdfa->lexeme[2] = '\0';
            pdfa->size = 2;
        }
        else
        {
            pdfa->lexeme[0] = '\0';
            pdfa->size = 0;
        }
        break;
    case A_LAST:
        assert(false);
        break;
    }
    print_lex_error(lerr, pdfa, ptok);
}

void init_dfa(Dfa* pdfa)
{
    pdfa->s = S_START;
    pdfa->line = 1;
    pdfa->col = 1;
    pdfa->lexeme = buff1;
    pdfa->trunc = false;
    buff1[0] = '\0';
    pdfa->size = 0;
    pdfa->error_count = 0;

}

void init_token(Token* ptok)
{
    ptok->line = 0;
    ptok->col = 0;
    ptok->lexeme = buff2;
    buff2[0] = '\0';
    ptok->size = 0;
    ptok->tid = T_ERR;
    ptok->num.f = 0.0;
}

tok_t keyword_check(const char* s)
// returns correct token based on keyword
{
    pch_int_hmap_node* p = pch_int_hmap_query(&keyword_hmap, s);
    if(p == NULL)
        return T_ID;
    else
        return p->value;
}

void post_process(Dfa* pdfa, Token* ptok)
{
    if((ptok->tid) == T_ID)
    {
        ptok->tid = keyword_check(ptok->lexeme);
        if((ptok->tid) == T_ID && (ptok->size) > 8)
            print_lex_error(LERR_LONG_ID, pdfa, ptok);
        if((ptok->tid) == T_ID && ptok->lexeme[0] == '_')
            print_lex_error(LERR_ID_UNSC, pdfa, ptok);
        ptok->num.f = 0.0;
    }
    else if(ptok->tid == T_NUM)
        ptok->num.i = atoi(ptok->lexeme);
    else if(ptok->tid == T_RNUM)
        ptok->num.f = atof(ptok->lexeme);
    else
        ptok->num.f = 0.0;
}

bool tick_dfa(char ch, Dfa* pdfa, Token* ptok, bool debug)
// return true if token is ready
{
    cclass_t cclass = get_cclass(ch);
    state_t s2 = state_table[pdfa->s][cclass];
    action_t a = action_table[pdfa->s][cclass];
    execute_action(a, ch, pdfa, ptok);
    if(debug)
        fprintf(stderr, "\t%s %c %s %s\n", STATE_STRS[pdfa->s], ch, STATE_STRS[s2], ACTION_STRS[a]);
    pdfa->s = s2;
    (pdfa->col)++;
    return (a == A_TOKS || a == A_TOKF || a == A_TOKN || a == A_ERR);
}

void get_token(FILE* fp, Dfa* pdfa, Token* ptok, bool debug)
{
    while(true)
    {
        char ch = get_character(fp);
        if(ch == '\0')
        {
            ptok->line = pdfa->line;
            ptok->col = pdfa->col;
            ptok->lexeme[0] = '\0';
            ptok->size = 0;
            ptok->tid = T_EOF;
            break;
        }
        else if(tick_dfa(ch, pdfa, ptok, debug))
        {
            post_process(pdfa, ptok);
            break;
        }
    }
}

void print_token(const Token* ptok, FILE* fp)
{
    fprintf(fp, "%2d %2d %s %s", ptok->line, ptok->col, TOK_STRS[ptok->tid], ptok->lexeme);
    if(ptok->tid == T_NUM)
        fprintf(fp, " %d\n", ptok->num.i);
    else if(ptok->tid == T_RNUM)
        fprintf(fp, " %lf\n", ptok->num.f);
    else
        fprintf(fp, "\n");
}

void print_token_sub(const Token* tok, FILE* fp)
{
	fprintf(fp, "%-15s %-10s %3d\n", TOK_STRS[tok->tid], tok->lexeme, tok->line);
}

// Driver program --------------------------------------------------------------

int lexer_main(FILE* ifp, FILE* ofp, int verbosity, token_printer tp)
{
    if(verbosity >= 4)
    {
        fprintf(stderr, "tokens:\n");
        for(int i=0; i<NUM_TOKENS; ++i)
            fprintf(stderr, "%s\n", TOK_STRS[i]);
        fprintf(stderr, "\ncclasses:\n");
        for(int i=0; i<NUM_CCLASSES; ++i)
            fprintf(stderr, "%s\n", CCLASS_STRS[i]);
        fprintf(stderr, "\nstates:\n");
        for(int i=0; i<NUM_STATES; ++i)
            fprintf(stderr, "%s\n", STATE_STRS[i]);
        fprintf(stderr, "\nactions:\n");
        for(int i=0; i<NUM_ACTIONS; ++i)
            fprintf(stderr, "%s\n", ACTION_STRS[i]);
    }

    init_lexer();

    if(verbosity >= 3)
    {
        int i, j;
        fprintf(stderr, "states: %d, cclasses: %d\n", NUM_STATES, NUM_CCLASSES);
        fprintf(stderr, "\nstate_table:\n");

        fprintf(stderr, "%7s", "");
        for(j=0; j<NUM_CCLASSES; ++j)
            fprintf(stderr, " %7s", CCLASS_STRS[j]);
        fputc('\n', stderr);
        for(i=0; i<NUM_STATES; ++i)
        {
            fprintf(stderr, "%7s", STATE_STRS[i]);
            for(j=0; j<NUM_CCLASSES; ++j)
                fprintf(stderr, " %7s", STATE_STRS[state_table[i][j]]);
            fputc('\n', stderr);
        }

        fprintf(stderr, "\naction_table:\n");
        fprintf(stderr, "%7s", "");
        for(j=0; j<NUM_CCLASSES; ++j)
            fprintf(stderr, " %7s", CCLASS_STRS[j]);
        fputc('\n', stderr);
        for(i=0; i<NUM_STATES; ++i)
        {
            fprintf(stderr, "%7s", STATE_STRS[i]);
            for(j=0; j<NUM_CCLASSES; ++j)
                fprintf(stderr, " %7s", ACTION_STRS[action_table[i][j]]);
            fputc('\n', stderr);
        }
    }

    Token tok;
    Dfa dfa;
    init_dfa(&dfa);
    init_token(&tok);
    bool debug = (verbosity >= 1);
    do
    {
        get_token(ifp, &dfa, &tok, debug);
        tp(&tok, ofp);
    }
    while(tok.tid != T_EOF);

    destroy_lexer();

    if(dfa.error_count > 0)
        return 1;
    else
        return 0;
}

void print_source_without_comments(FILE * ifp, FILE * ofp){
	int state = 0;
	char ch = get_character(ifp);
	int bch;
	int transition_mat[][4] = {{0,1},{0,2},{2,3},{2,0}};
	int action_mat[][4] = {{1,0},{2,0},{0,0},{0,0}};
	while(ch != '\0'){
		if(ch == '*') bch = 1;
		else bch = 0;
		if(action_mat[state][bch] == 0);
		else if(action_mat[state][bch] == 1) put_character(ofp, ch);
		else if(action_mat[state][bch] == 2){
			put_character(ofp, '*');
			put_character(ofp, ch);
		}
		state = transition_mat[state][bch];
		ch = get_character(ifp);
	}
	put_character(ofp,ch);
}

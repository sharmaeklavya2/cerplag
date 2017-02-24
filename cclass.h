#ifndef H_CCLASS
#define H_CCLASS

typedef enum
{
    C_REST,     // restricted
    C_ALPHA,    // alphabet
    C_E,        // e
    C_UNSC,     // underscore
    C_DIG,      // digits
    C_WS,       // whitespace
    C_SCO,      // single-character operator
    C_PM,       // '+' or '-'
    C_EQUALS,   // '='
    C_EXCL,     // '!'
    C_DOT,      // '.'
    C_STAR,     // '*'
    C_COLON,    // ':'
    C_LT,       // '<'
    C_GT,       // '>'
    C_EOF,      // EOF (end of file)
    C_LAST
}cclass_t;

#define NUM_CCLASS C_LAST

void precompute_cclass();
cclass_t get_cclass(char ch);

#endif  // H_CCLASS

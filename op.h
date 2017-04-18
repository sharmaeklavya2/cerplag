#ifndef H_OP
#define H_OP

typedef enum {
    OP_PLUS, OP_MINUS,
    OP_LT, OP_GT,
    OP_LE, OP_GE,
    OP_EQ, OP_NE,
    OP_MUL, OP_DIV,
    OP_AND, OP_OR,
    OP_LAST
} op_t;

#define NUM_OP OP_LAST

#define OP_STRS_LIST {"+", "-", "<", ">", "<=", ">=", "==", "!=", "*", "/", "AND", "OR"}

extern char* OP_STRS[];

#endif  // H_OP

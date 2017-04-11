#ifndef H_AST_DEFS
#define H_AST_DEFS

#include <stdbool.h>

typedef enum {
#define X(a, b) ASTN_##a,
#include "data/ast_nodes.xmac"
#undef X
    ASTN_LAST
} astn_t;

#define NUM_ASTN ASTN_LAST

typedef enum {TYPE_INTEGER, TYPE_REAL, TYPE_BOOLEAN} valtype_t;
#define TYPE_STRS_LIST {"int", "real", "bool"}
#define TYPE_SIZES_LIST {sizeof(int), sizeof(double), sizeof(bool)}

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

#define OP_STRS_LIST {"+", "-", "<", ">", "<=", ">=", "==", "!=", "*", "/", "&", "|"}

typedef struct {
    astn_t node_type;
    valtype_t type;
    int size;
    int line, col;
} BaseAstNode;

typedef struct {BaseAstNode base;} AstNode;
typedef AstNode* pAstNode;

#define X(a, b) typedef struct {BaseAstNode base; b} a##Node;
#include "data/ast_nodes.xmac"
#undef X

extern char* ASTN_STRS[];
extern char* TYPE_STRS[];
extern int TYPE_SIZES[];
extern int TYPE_ALIGNS[];
extern char* OP_STRS[];

#endif  // H_AST_DEFS

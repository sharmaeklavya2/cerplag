#ifndef H_AST_DEFS
#define H_AST_DEFS

#include <stdbool.h>
#include "op.h"
#include "type.h"
#include "ircode.h"

typedef enum {
#define X(a, b) ASTN_##a,
#include "ast_nodes.xmac"
#undef X
    ASTN_LAST
} astn_t;

#define NUM_ASTN ASTN_LAST

struct AddrNode;

typedef struct {
    astn_t node_type;
    valtype_t type;
    int size;
    int line, col;
    struct AddrNode* addr;
    struct IRCode ircode;
} BaseAstNode;

typedef struct AstNode {BaseAstNode base;} AstNode;
typedef AstNode* pAstNode;

#define X(a, b) typedef struct a##Node {BaseAstNode base; b} a##Node;
#include "ast_nodes.xmac"
#undef X

extern char* ASTN_STRS[];
extern char* TYPE_STRS[];
extern int TYPE_SIZES[];
extern int TYPE_ALIGNS[];
extern char* OP_STRS[];

#include "addr.h"

#endif  // H_AST_DEFS

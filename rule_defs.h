#ifndef H_RULE_DEFS
#define H_RULE_DEFS

#include "bitset.h"

#define NUM_RULES 100

typedef struct RuleNode
{
    struct RuleNode* next;
    gsymb_t value;
    bitset_t first;
} RuleNode;

typedef struct
{
    gsymb_t lhs;
    RuleNode* head;
    RuleNode* tail;
}Rule;

#endif  // H_RULE_DEFS

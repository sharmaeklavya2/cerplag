#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void CLASSED(init)(CLASS* plist) {
    plist->first = NULL;
    plist->last = NULL;
    plist->size = 0;
}

void CLASSED(copy)(CLASS* dest, CLASS* src) {
    dest->first = src->first;
    dest->last = src->last;
    dest->size = src->size;
}

void CLASSED(combine)(CLASS* plist, CLASS* p1, CLASS* p2) {
    plist->size = p1->size + p2->size;
    if(p1->first == NULL) {
        plist->first = p2->first;
        plist->last = p2->last;
    }
    else if(p2->first == NULL) {
        plist->first = p1->first;
        plist->last = p1->last;
    }
    else {
        TYPED(link)(p1->last, p2->first);
        plist->first = p1->first;
        plist->last = p2->last;
    }
}

void CLASSED(prepend)(CLASS* plist, TYPE instr) {
    (plist->size)++;
    if(plist->first == NULL) {
        plist->first = plist->last = instr;
        plist->size = 1;
    }
    else {
        TYPED(link)(instr, plist->first);
        plist->first = instr;
    }
}

void CLASSED(append)(CLASS* plist, TYPE instr) {
    (plist->size)++;
    if(plist->first == NULL) {
        plist->first = plist->last = instr;
        plist->size = 1;
    }
    else {
        TYPED(link)(plist->last, instr);
        plist->last = instr;
    }
}

void CLASSED(remove)(CLASS* plist, TYPE elem) {
    (plist->size)--;
    // first manipulate pointers of list
    if(plist->first == NULL) {
        fprintf(stderr, "Cannot remove from empty linked-list.\n");
        plist->first = plist->last = elem;
        plist->size = 0;
    }
    else if(plist->first == elem) {
        if(plist->last == elem) {
            plist->first = plist->last = NULL;
        }
        else {
            plist->first = plist->first->next;
        }
    }
    else if(plist->last == elem) {
        plist->last = plist->last->prev;
    }
    // now detach elem
    TYPE prev = elem->prev;
    TYPE next = elem->next;
    elem->prev = elem->next = NULL;
    if(next != NULL) {
        next->prev = prev;
    }
    if(prev != NULL) {
        prev->next = next;
    }
    TYPED(destroy)(elem);
}

void CLASSED(clear)(CLASS* plist) {
    TYPED(destroy_list)(plist->first);
    plist->first = plist->last = NULL;
    plist->size = 0;
}

void CLASSED(print)(CLASS* plist, FILE* fp) {
    int i;
    TYPE node = plist->first;
    for(i=0; i<(plist->size); ++i) {
        TYPED(print)(node, fp);
        fprintf(fp, "\n");
        node = node->next;
    }
}

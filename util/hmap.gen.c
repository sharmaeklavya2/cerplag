#include <stdlib.h>
#include "pch.h"

ITYPED(hmap_node)* ITYPED(hmap_get_node)(KTYPE k, VTYPE v, ITYPED(hmap_node)* next)
{
    ITYPED(hmap_node)* n = malloc(sizeof(ITYPED(hmap_node)));
    n->key = k;
    n->value = v;
    n->next = next;
    return n;
}

void ITYPED(hmap_destroy_chain)(ITYPED(hmap_node)* p)
{
    ITYPED(hmap_node)* p2 = NULL;
    while(p != NULL)
    {
        p2 = p->next;
        p->next = NULL;
        free(p);
        p = p2;
    }
}

void ITYPED(hmap_init)(ITYPED(hmap)* phmap, int capacity)
{
    phmap->plist = calloc(capacity, sizeof(ITYPED(hmap_node)*));
    phmap->capacity = capacity;
    phmap->size = 0;
}

void ITYPED(hmap_destroy)(ITYPED(hmap)* phmap)
{
    int i;
    for(i=0; i < (phmap->capacity); ++i)
        ITYPED(hmap_destroy_chain)(phmap->plist[i]);
    free(phmap->plist);
}

ITYPED(hmap_node)* ITYPED(hmap_query)(ITYPED(hmap)* phmap, KTYPE k)
{
    unsigned h = KTYPED(hash)(k) % (phmap->capacity);
    ITYPED(hmap_node)* n = phmap->plist[h];
    while(n != NULL && !(KTYPED(equals)(n->key, k)))
        n = n->next;
    return n;
}

void ITYPED(hmap_update)(ITYPED(hmap)* phmap, KTYPE k, VTYPE v)
{
    unsigned h = KTYPED(hash)(k) % (phmap->capacity);
    phmap->plist[h] = ITYPED(hmap_get_node)(k, v, phmap->plist[h]);
    (phmap->size)++;
}

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

ITYPED(hmap_node)* ITYPED(hmap_get_node)(KTYPE k, VTYPE v, ITYPED(hmap_node)* next)
{
    ITYPED(hmap_node)* n = malloc(sizeof(ITYPED(hmap_node)));
    n->key = k;
    n->value = v;
    n->next = next;
    return n;
}

void ITYPED(hmap_destroy_chain)(ITYPED(hmap_node)* p, bool destroy_key, bool destroy_value)
{
    ITYPED(hmap_node)* p2 = NULL;
    while(p != NULL)
    {
        p2 = p->next;
        p->next = NULL;
        if(destroy_key)
            KTYPED(destroy)(p->key);
        if(destroy_value)
            VTYPED(destroy)(p->value);
        free(p);
        p = p2;
    }
}

void ITYPED(hmap_init)(ITYPED(hmap)* phmap, int capacity, bool destroy_key)
{
    phmap->plist = calloc(capacity, sizeof(ITYPED(hmap_node)*));
    phmap->capacity = capacity;
    phmap->size = 0;
    phmap->destroy_key = destroy_key;
    phmap->destroy_value = true;
}

void ITYPED(hmap_destroy)(ITYPED(hmap)* phmap)
{
    int i;
    for(i=0; i < (phmap->capacity); ++i)
        ITYPED(hmap_destroy_chain)(phmap->plist[i], phmap->destroy_key, phmap->destroy_value);
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

VTYPE ITYPED(hmap_get)(ITYPED(hmap)* phmap, KTYPE k)
{
    ITYPED(hmap_node)* n = ITYPED(hmap_query)(phmap, k);
    if(n == NULL) {
        fprintf(stderr, "KeyError: ");
        KTYPED(print)(k, stderr);
        fprintf(stderr, "\n");
        abort();
    }
    return n->value;
}

ITYPED(hmap_node)* ITYPED(hmap_insert)(ITYPED(hmap)* phmap, KTYPE k, VTYPE v)
{
    const int lf_num = 3, lf_den = 4;
    if(lf_den * (phmap->size+1) > lf_num * phmap->capacity)
        ITYPED(hmap_rehash)(phmap, 2 * (phmap->capacity));
    unsigned h = KTYPED(hash)(k) % (phmap->capacity);
    ITYPED(hmap_node)* n = phmap->plist[h];
    while(n != NULL && !(KTYPED(equals)(n->key, k)))
        n = n->next;
    if(n == NULL) {
        (phmap->size)++;
        n = phmap->plist[h] = ITYPED(hmap_get_node)(k, v, phmap->plist[h]);
    }
    return n;
}


ITYPED(hmap_node)* ITYPED(hmap_update)(ITYPED(hmap)* phmap, KTYPE k, VTYPE v)
{
    int old_size = phmap->size;
    ITYPED(hmap_node)* n = ITYPED(hmap_insert)(phmap, k, v);
    if(phmap->size == old_size) {
        if(phmap->destroy_value)
            VTYPED(destroy)(n->value);
        n->value = v;
    }
    return n;
}

double ITYPED(hmap_load_factor)(ITYPED(hmap)* phmap)
{return (double)(phmap->size) / (phmap->capacity);}

void ITYPED(hmap_rehash)(ITYPED(hmap)* phmap, int new_capacity)
{
    int i;
    ITYPED(hmap_node)** plist2 = calloc(new_capacity, sizeof(ITYPED(hmap_node)*));
    for(i=0; i<(phmap->capacity); ++i) {
        ITYPED(hmap_node)* n = phmap->plist[i];
        while(n != NULL) {
            unsigned h = KTYPED(hash)(n->key) % new_capacity;
            plist2[h] = ITYPED(hmap_get_node)(n->key, n->value, plist2[h]);
            n = n->next;
        }
        ITYPED(hmap_destroy_chain)(phmap->plist[i], false, false);
    }

    free(phmap->plist);
    phmap->plist = plist2;
    phmap->capacity = new_capacity;
}

void ITYPED(hmap_print)(ITYPED(hmap)* phmap, FILE* fp) {
    int capacity = phmap->capacity;
    ITYPED(hmap_node)** plist = phmap->plist;
    ITYPED(hmap_node)* n = NULL;
    fprintf(fp, "hmap(size=%d, capacity=%d):\n", phmap->size, capacity);
    for(int i=0; i<capacity; ++i) {
        for(n = plist[i]; n != NULL; n = n->next) {
            fprintf(fp, "  ");
            KTYPED(print)(n->key, fp);
            fprintf(fp, ": ");
            VTYPED(print)(n->value, fp);
            fprintf(fp, "\n");
        }
    }
}

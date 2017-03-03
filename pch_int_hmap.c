/* Batch no. 13
2014A7PS130P: Eklavya Sharma
2014A7PS023P: Daivik Nema */
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "pch_int_hmap.h"

#define KTYPE pch
#define VTYPE int

#define KTYPED(x) pch_##x
#define VTYPED(x) int_##x
#define ITYPED(x) pch_int_##x

#include "hmap.gen.c"

#undef KTYPE
#undef VTYPE
#undef KTYPED
#undef VTYPED
#undef ITYPED

/*
#include <stdio.h>
#include <string.h>
int main()
{
    int i, n;
    pch_int_hmap ht;
    pch_int_hmap_init(&ht, 10);
    fprintf(stderr, "hmap inited\n");
    char* keys[] = {"one", "two", "three", "four", "five"};
    int values[] = {10, 20, 30, 40, 50};
    char query[100];

    n = sizeof(values) / sizeof(int);
    for(i=0; i<n; ++i)
    {
        pch_int_hmap_update(&ht, keys[i], values[i]);
        fprintf(stderr, "added (%s, %d)\n", keys[i], values[i]);
    }

    while(true)
    {
        int res = scanf("%s", query);
        if(res == EOF)
            break;
        pch_int_hmap_node* p = pch_int_hmap_query(&ht, query);
        if(p == NULL)
            printf("not found\n");
        else
            printf("%d\n", p->value);
    }
    return 0;
}
*/

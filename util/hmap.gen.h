struct ITYPED(hmap_node_)
{
    struct ITYPED(hmap_node_)* next;
    KTYPE key;
    VTYPE value;
};

typedef struct ITYPED(hmap_node_) ITYPED(hmap_node);

ITYPED(hmap_node)* ITYPED(hmap_get_node)(KTYPE k, VTYPE v, ITYPED(hmap_node)* next);

typedef struct
{
    ITYPED(hmap_node)** plist;
    int capacity;
    int size;
}ITYPED(hmap);

void ITYPED(hmap_init)(ITYPED(hmap)* phmap, int size);

void ITYPED(hmap_destroy)(ITYPED(hmap)* phmap);

ITYPED(hmap_node)* ITYPED(hmap_query)(ITYPED(hmap)* phmap, KTYPE k);

VTYPE ITYPED(hmap_get)(ITYPED(hmap)* phmap, KTYPE k);

void ITYPED(hmap_update)(ITYPED(hmap)* phmap, KTYPE k, VTYPE v);

double ITYPED(hmap_load_factor)(ITYPED(hmap)* phmap);

void ITYPED(hmap_rehash)(ITYPED(hmap)* phmap, int new_capacity);

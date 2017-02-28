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
    int size;
    int count;
}ITYPED(hmap);

void ITYPED(hmap_init)(ITYPED(hmap)* phmap, int size);

void ITYPED(hmap_destroy)(ITYPED(hmap)* phmap);

ITYPED(hmap_node)* ITYPED(hmap_query)(ITYPED(hmap)* phmap, KTYPE k);

void ITYPED(hmap_update)(ITYPED(hmap)* phmap, KTYPE k, VTYPE v);

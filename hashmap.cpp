#include "include/hashmap.h"

HNode *hm_lookup(HMap *hmap, HNode *key, bool (*eq)(HNode *, HNode *)) {
    hm_help_rehashing(hmap);
    HNode **from = h_lookup(&hmap->newer, key, eq);
    if (!from) {
        from = h_lookup(&hmap->older, key, eq);
    }
    return from ? *from : NULL;
}

void hm_insert(HMap *hmap, HNode *node) {
    if (!hmap->newer.tab) {
        h_init(&hmap->newer, 4); // initialized it if empty
    }
    h_insert(&hmap->newer, node); // always insert to the newer table
    // don't trigger if the older table already exists (meaning it's already being rehashed)
    if (!hmap->older.tab) {
        // check whether we need to rehash
        // TODO: should k_max_load_factor depend on size?
        if (const size_t threshold = (hmap->newer.mask + 1) * k_max_load_factor; hmap->newer.size >= threshold) {
            hm_trigger_rehashing(hmap);
        }
    }
    hm_help_rehashing(hmap); // migrate some keys
}

HNode *hm_delete(HMap *hmap, HNode *key, bool (*eq)(HNode *, HNode *)) {
    hm_help_rehashing(hmap);
    if (HNode **from = h_lookup(&hmap->newer, key, eq)) {
        return h_detach(&hmap->newer, from);
    }
    if (HNode **from = h_lookup(&hmap->older, key, eq)) {
        return h_detach(&hmap->older, from);
    }
    return NULL;
}

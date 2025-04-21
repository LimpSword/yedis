#ifndef HASHMAP_H
#define HASHMAP_H
#include <cassert>
#include <cstdint>
#include <cstdlib>

struct HNode {
    HNode *next = nullptr;
    uint64_t hcode = 0; // hash value
};

struct HTab {
    HNode **tab = nullptr; // array of slots
    size_t mask = 0; // power of 2 array size, 2^n - 1
    size_t size = 0; // number of keys
};

static void h_init(HTab *htab, const size_t n) {
    assert(n > 0 && ((n - 1) & n) == 0); // n must be a power of 2
    htab->tab = static_cast<HNode **>(calloc(n, sizeof(HNode *))); // malloc setting to 0 dynamically
    htab->mask = n - 1;
    htab->size = 0;
}

static void h_insert(HTab *htab, HNode *node) {
    // find a position within htab bounds
    const size_t pos = node->hcode & htab->mask; // node->hcode & (n - 1)
    HNode *next = htab->tab[pos];
    node->next = next;
    htab->tab[pos] = node;
    htab->size++;
}

static HNode **h_lookup(const HTab *htab, HNode *key, bool (*eq)(HNode *, HNode *)) {
    if (!htab->tab) {
        return NULL;
    }
    const size_t pos = key->hcode & htab->mask;
    HNode **from = &htab->tab[pos]; // incoming pointer to the target
    for (HNode *cur; (cur = *from) != NULL; from = &cur->next) {
        if (cur->hcode == key->hcode && eq(cur, key)) {
            return from; // Why not return `cur`? We need the parent for deletion
        }
    }
    return NULL;
}

static HNode *h_detach(HTab *htab, HNode **from) {
    HNode *node = *from; // the target node
    *from = node->next; // update the incoming pointer to the target
    htab->size--;
    return node;
}

struct HMap {
    HTab newer;
    HTab older;
    size_t migrate_pos = 0;
};

HNode *hm_lookup(HMap *hmap, HNode *key, bool (*eq)(HNode *, HNode *));

void hm_insert(HMap *hmap, HNode *node);

HNode *hm_delete(HMap *hmap, HNode *key, bool (*eq)(HNode *, HNode *));

static void hm_trigger_rehashing(HMap *hmap) {
    hmap->older = hmap->newer; // (newer, older) <- (new_table, newer)
    h_init(&hmap->newer, (hmap->newer.mask + 1) * 2);
    hmap->migrate_pos = 0;
}


constexpr size_t k_max_load_factor = 8;
constexpr size_t k_rehashing_work = 128; // constant work


static void hm_help_rehashing(HMap *hmap) {
    size_t nwork = 0;
    while (nwork < k_rehashing_work && hmap->older.size > 0) {
        // find a non-empty slot
        HNode **from = &hmap->older.tab[hmap->migrate_pos];
        if (!*from) {
            hmap->migrate_pos++;
            continue; // empty slot
        }
        // move the first list item to the newer table
        h_insert(&hmap->newer, h_detach(&hmap->older, from));
        nwork++;
    }
    // discard the old table if done
    if (hmap->older.size == 0 && hmap->older.tab) {
        free(hmap->older.tab);
        hmap->older = HTab{};
    }
}

#endif //HASHMAP_H

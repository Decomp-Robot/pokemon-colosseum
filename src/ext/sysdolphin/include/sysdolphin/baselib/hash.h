#ifndef SYSDOLPHIN_BASELIB_HASH_H
#define SYSDOLPHIN_BASELIB_HASH_H

#include <dolphin/types.h>

typedef struct _HSD_HashEntry HSD_HashEntry;
typedef struct _HSD_Hash HSD_Hash;

/* Every class in the base library starts with the shared class interface. A
 * hash table class appends the two methods that decide which bucket a key
 * belongs to and when two keys are the same key. */
typedef struct _HSD_HashClass {
    /* 0x00 */ u8 base[60];
    /* 0x3C */ u32 (*get_index)(HSD_Hash* hash, void* key);
    /* 0x40 */ int (*compare)(HSD_Hash* hash, void* entry_key, void* key);
} HSD_HashClass;

/* One entry of a bucket chain. */
struct _HSD_HashEntry {
    /* 0x00 */ HSD_HashEntry* next;
    /* 0x04 */ void* key;
    /* 0x08 */ void* value;
};

struct _HSD_Hash {
    /* 0x00 */ HSD_HashClass* class;
    /* 0x04 */ HSD_HashEntry** table;
    /* 0x08 */ u32 table_size;
};

void* HSD_HashSearch(HSD_Hash* hash, void* key, u32* found);

#endif

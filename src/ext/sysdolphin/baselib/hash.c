#include <sysdolphin/baselib/hash.h>

#include <sysdolphin/baselib/debug.h>

void* HSD_HashSearch(HSD_Hash* hash, void* key, u32* found)
{
    HSD_HashEntry* entry;
    HSD_HashEntry* found_entry;
    u32 idx;

    idx = hash->class->get_index(hash, key);
    HSD_ASSERT(113, idx < hash->table_size);
    if (hash->table[idx] == 0) {
        entry = 0;
    } else {
        entry = hash->table[idx];
        while (entry != 0) {
            if (hash->class->compare(hash, entry->key, key) == 0) {
                found_entry = entry;
                goto found;
            }
            entry = entry->next;
        }
        entry = 0;
    }
found:
    found_entry = entry;
    if (found != 0) {
        *found = found_entry != 0;
    }
    return found_entry != 0 ? found_entry->value : 0;
}

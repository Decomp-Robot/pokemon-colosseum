#include <string.h>

#include <sysdolphin/baselib/debug.h>
#include <sysdolphin/baselib/id.h>
#include <sysdolphin/baselib/objalloc.h>

static HSD_IDTable id_table;
static HSD_ObjAllocData id_alloc_data;

void HSD_IDClear(void)
{
    memset(&id_table, 0, sizeof(id_table));
}

void* HSD_IDGet(HSD_IDTable* table, u32 id, u32* found)
{
    HSD_IDEntry* entry;

    if (table == NULL) {
        table = &id_table;
    }
    entry = table->buckets[id % HSD_ID_TABLE_SIZE];
    while (entry != NULL) {
        if (entry->id == id) {
            if (found != NULL) {
                *found = 1;
            }
            return entry->data;
        }
        entry = entry->next;
    }
    if (found != NULL) {
        *found = 0;
    }
    return NULL;
}

void HSD_IDRemove(HSD_IDTable* table, u32 id)
{
    HSD_IDEntry* entry;
    HSD_IDEntry* prev;
    u32 bucket;
    u32 hash;

    if (table == NULL) {
        table = &id_table;
    }
    hash = id % HSD_ID_TABLE_SIZE;
    bucket = hash;
    entry = table->buckets[bucket];
    prev = NULL;
    while (entry != NULL) {
        if (entry->id == id) {
            if (prev != NULL) {
                prev->next = entry->next;
            } else {
                table->buckets[bucket] = entry->next;
            }
            HSD_ObjFree(&id_alloc_data, entry);
            return;
        }
        prev = entry;
        entry = entry->next;
    }
}

void HSD_IDSet(HSD_IDTable* table, u32 id, void* data)
{
    HSD_IDEntry* cur;
    HSD_IDEntry* entry;
    u64 hash;
    u32 bucket;

    if (table == NULL) {
        table = &id_table;
    }
    hash = id % HSD_ID_TABLE_SIZE;
    bucket = hash;
    cur = table->buckets[bucket];
    while (cur != NULL) {
        if (cur->id == id) {
            break;
        }
        cur = cur->next;
    }
    if (cur != NULL) {
        cur->id = id;
        cur->data = data;
    } else {
        entry = HSD_ObjAlloc(&id_alloc_data);
        HSD_ASSERT(67, entry);
        memset(entry, 0, sizeof(HSD_IDEntry));
        entry->id = id;
        entry->data = data;
        entry->next = table->buckets[bucket];
        cur = entry;
        table->buckets[bucket] = cur = entry;
    }
}

void HSD_IDReset(void)
{
    memset(&id_table, 0, sizeof(id_table));
}

void HSD_IDInit(void)
{
    HSD_ObjAllocInit(&id_alloc_data, sizeof(HSD_IDEntry), __alignof__(HSD_IDEntry));
}

HSD_ObjAllocData* HSD_IDGetAllocData(void)
{
    return &id_alloc_data;
}

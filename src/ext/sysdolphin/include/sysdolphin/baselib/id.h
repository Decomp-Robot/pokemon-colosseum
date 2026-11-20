#ifndef SYSDOLPHIN_BASELIB_ID_H
#define SYSDOLPHIN_BASELIB_ID_H

#include <dolphin/types.h>
#include <sysdolphin/baselib/objalloc.h>

#define HSD_ID_TABLE_SIZE 101

typedef struct _HSD_IDEntry {
    struct _HSD_IDEntry* next;
    u32 id;
    void* data;
} HSD_IDEntry;

typedef struct _HSD_IDTable {
    HSD_IDEntry* buckets[HSD_ID_TABLE_SIZE];
} HSD_IDTable;

void HSD_IDClear(void);
void* HSD_IDGet(HSD_IDTable* table, u32 id, u32* found);
void HSD_IDRemove(HSD_IDTable* table, u32 id);
void HSD_IDSet(HSD_IDTable* table, u32 id, void* data);
void HSD_IDReset(void);
void HSD_IDInit(void);
HSD_ObjAllocData* HSD_IDGetAllocData(void);

#endif

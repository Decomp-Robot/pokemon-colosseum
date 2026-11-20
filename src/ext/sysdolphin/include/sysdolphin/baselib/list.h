#ifndef SYSDOLPHIN_BASELIB_LIST_H
#define SYSDOLPHIN_BASELIB_LIST_H

#include <dolphin/types.h>

#include <sysdolphin/baselib/objalloc.h>

typedef struct _HSD_SList {
    struct _HSD_SList* next;
    void* data;
} HSD_SList;

typedef struct _HSD_DList {
    struct _HSD_DList* next;
    struct _HSD_DList* prev;
    void* data;
} HSD_DList;

HSD_SList* HSD_SListRemove(HSD_SList* node);
HSD_SList* HSD_SListPrepend(HSD_SList* next, void* data);
HSD_SList* HSD_SListAlloc(void);

HSD_ObjAllocData* HSD_DListGetAllocData(void);
HSD_ObjAllocData* HSD_SListGetAllocData(void);

void HSD_ListInit(void);

#endif

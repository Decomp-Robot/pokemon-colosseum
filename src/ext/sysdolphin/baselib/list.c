#include <string.h>

#include <sysdolphin/baselib/list.h>

#include <sysdolphin/baselib/debug.h>

static HSD_ObjAllocData dlist_alloc_data;
static HSD_ObjAllocData slist_alloc_data;

#define HSD_LIST_ALLOC(list, alloc_data, obj_type)                              \
    do {                                                                       \
        (list) = HSD_ObjAlloc(alloc_data);                                     \
        HSD_ASSERT(76, list);                                                  \
        memset(list, 0, sizeof(obj_type));                                     \
    } while (0)

static HSD_DList* dlist_alloc(void)
{
    HSD_DList* prev;

    HSD_LIST_ALLOC(prev, &dlist_alloc_data, HSD_DList);
    return prev;
}

HSD_SList* HSD_SListRemove(HSD_SList* node)
{
    if (node != NULL) {
        HSD_SList* next = node->next;
        HSD_ObjFree(&slist_alloc_data, node);
        return next;
    }
    return NULL;
}

HSD_SList* HSD_SListPrepend(HSD_SList* next, void* data)
{
    HSD_SList* list;
    HSD_SList* prev;

    HSD_LIST_ALLOC(list, &slist_alloc_data, HSD_SList);
    prev = list;
    prev->data = data;
    HSD_ASSERT(202, prev);
    prev->next = next;
    return prev;
}

HSD_SList* HSD_SListAlloc(void)
{
    HSD_SList* list;

    HSD_LIST_ALLOC(list, &slist_alloc_data, HSD_SList);
    return list;
}

HSD_ObjAllocData* HSD_DListGetAllocData(void)
{
    return &dlist_alloc_data;
}

HSD_ObjAllocData* HSD_SListGetAllocData(void)
{
    return &slist_alloc_data;
}

void HSD_ListInit(void)
{
    HSD_ObjAllocInit(&slist_alloc_data, sizeof(HSD_SList), 4);
    HSD_ObjAllocInit(&dlist_alloc_data, sizeof(HSD_DList), 4);
}

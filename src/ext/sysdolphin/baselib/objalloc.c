#include <string.h>

#include <sysdolphin/baselib/debug.h>
#include <sysdolphin/baselib/memory.h>
#include <sysdolphin/baselib/objalloc.h>
#include <sysdolphin/baselib/util.h>

#define OBJ_ALLOC_NO_LIMIT (~0u)

static HSD_ObjAllocData* objAllocRegistry;
static struct {
    u32 reserved;
    u32 reserved_current;
    u32 arena_size;
    u32 arena_size_current;
} objAllocSetup = { 0, 0, OBJ_ALLOC_NO_LIMIT, OBJ_ALLOC_NO_LIMIT };

void HSD_ObjAllocResetRegistry(void)
{
    objAllocRegistry = NULL;
}

void HSD_ObjAllocInit(HSD_ObjAllocData* data, u32 size, u32 align)
{
    HSD_ASSERT(430, data);

    if (data != NULL) {
        HSD_ObjAllocData** link = &objAllocRegistry;
        while (*link) {
            if (*link == data) {
                *link = (*link)->next;
            } else {
                link = &(*link)->next;
            }
        }
    } else {
        objAllocRegistry = NULL;
    }

    memset(data, 0, sizeof(HSD_ObjAllocData));
    data->num_alloc_limit = OBJ_ALLOC_NO_LIMIT;
    data->size_reserved = 0;
    data->size_limit = OBJ_ALLOC_NO_LIMIT;

    HSD_ASSERT(441, align <= 32);
    HSD_ASSERT(442, HSD_GetNbBits(align) == 1);

    data->obj_align = align;
    data->obj_size = (size + (data->obj_align - 1)) & ~(data->obj_align - 1);
    data->next = objAllocRegistry;
    objAllocRegistry = data;
}

void HSD_ObjFree(HSD_ObjAllocData* data, void* obj)
{
    data->num_alloc--;
    HSD_Free(obj);
}

void* HSD_ObjAlloc(HSD_ObjAllocData* data)
{
    if (data->limit_enabled && data->num_alloc >= data->num_alloc_limit) {
        return NULL;
    }
    data->num_alloc++;
    if (data->num_alloc > data->num_alloc_max) {
        data->num_alloc_max = data->num_alloc;
    }
    return HSD_Alloc(data->obj_size);
}

void HSD_ObjAllocSetArenaConfig(u32 arena_size, u32 reserved)
{
    objAllocSetup.reserved = objAllocSetup.reserved_current = reserved;
    objAllocSetup.arena_size = objAllocSetup.arena_size_current = arena_size;
}

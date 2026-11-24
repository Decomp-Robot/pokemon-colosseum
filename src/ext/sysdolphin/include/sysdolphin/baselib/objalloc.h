#ifndef SYSDOLPHIN_BASELIB_OBJALLOC_H
#define SYSDOLPHIN_BASELIB_OBJALLOC_H

#include <dolphin/types.h>

/* State of one fixed-size object allocator. The size and alignment handed to
 * HSD_ObjAllocInit describe the objects it hands out, and allocators are
 * chained through next. */
typedef struct _HSD_ObjAllocData {
    u8 limit_enabled : 1; /* honour num_alloc_limit while set */
    u32 unk_04;
    u32 num_alloc;      /* live objects */
    u32 unk_0C;
    u32 num_alloc_max;  /* high-water mark */
    u32 num_alloc_limit;
    u32 size_reserved;
    u32 size_limit;
    u32 obj_size;       /* object size rounded up to obj_align */
    u32 obj_align;
    struct _HSD_ObjAllocData* next;
} HSD_ObjAllocData;

void HSD_ObjAllocResetRegistry(void);
void HSD_ObjAllocInit(HSD_ObjAllocData* data, u32 size, u32 align);
void HSD_ObjFree(HSD_ObjAllocData* data, void* obj);
void* HSD_ObjAlloc(HSD_ObjAllocData* data);
void HSD_ObjAllocSetArenaConfig(u32 arena_size, u32 reserved);

#endif

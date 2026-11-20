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
    u32 unk_18;
    u32 unk_1C;
    u32 obj_size;       /* object size rounded up to obj_align */
    u32 obj_align;
    struct _HSD_ObjAllocData* next;
} HSD_ObjAllocData;

void HSD_ObjAllocInit(HSD_ObjAllocData* alloc_data, u32 obj_size, u32 obj_align);
void HSD_ObjFree(HSD_ObjAllocData* alloc_data, void* obj);
void* HSD_ObjAlloc(HSD_ObjAllocData* alloc_data);

#endif

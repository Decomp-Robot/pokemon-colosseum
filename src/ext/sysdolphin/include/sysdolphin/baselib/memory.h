#ifndef SYSDOLPHIN_BASELIB_MEMORY_H
#define SYSDOLPHIN_BASELIB_MEMORY_H

#include <dolphin/types.h>

/* Allocate a size-byte block through the installed SysDolphin allocator. */
void* HSD_Alloc(u32 size);

/* Release a block through the installed SysDolphin allocator. */
void HSD_Free(void* ptr);

#endif

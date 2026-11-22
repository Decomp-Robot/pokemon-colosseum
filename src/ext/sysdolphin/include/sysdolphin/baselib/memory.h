#ifndef SYSDOLPHIN_BASELIB_MEMORY_H
#define SYSDOLPHIN_BASELIB_MEMORY_H

#include <dolphin/types.h>

/* The allocator interface installed by the game's memory system. */
typedef struct __mem_cb {
    void* (*alloc)(u32 size, u32 align, u32 param); /* allocate a block */
    void (*free)(void* ptr);                       /* release a block */
    s32 (*reset_heap)(void);                       /* recreate the active heap */
    s32 (*check_heap)(void);                       /* check the active heap for corruption */
    BOOL (*is_heap_mem)(void* ptr);                /* test heap membership */
} __mem_cb;

/* Allocate a size-byte block through the installed SysDolphin allocator. */
void* HSD_Alloc(u32 size);

/* Release a block through the installed SysDolphin allocator. */
void HSD_Free(void* ptr);

/* Test whether a block belongs to the currently installed heap. */
BOOL HSD_IsHeapMem(void* ptr);

/* Install the allocator callbacks and the size of their table. */
void HSD_SetMemCB(__mem_cb* cb, u32 size);

#endif

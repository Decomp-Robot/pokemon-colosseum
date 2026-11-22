#include <sysdolphin/baselib/memory.h>

#include <sysdolphin/baselib/debug.h>

static __mem_cb mem_cb;

void* HSD_Alloc(u32 size)
{
    return mem_cb.alloc(size, 32, 0);
}

void HSD_Free(void* ptr)
{
    mem_cb.free(ptr);
}

BOOL HSD_IsHeapMem(void* ptr)
{
    return mem_cb.is_heap_mem(ptr);
}

void HSD_SetMemCB(__mem_cb* cb, u32 size)
{
    HSD_ASSERT(20, size == sizeof(__mem_cb));
    mem_cb = *cb;
}

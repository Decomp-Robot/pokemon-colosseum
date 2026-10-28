#include <dolphin.h>
#include <dolphin/os.h>

// undefine the macros so they do not error the file.
#undef OSPhysicalToCached
#undef OSPhysicalToUncached
#undef OSCachedToPhysical
#undef OSUncachedToPhysical
#undef OSCachedToUncached
#undef OSUncachedToCached

// UNUSED: the game never calls this; dead-stripped at link.
void * OSPhysicalToCached(u32 paddr) {
    ASSERTMSGLINE(0x2C, paddr < 0x10000000U, "OSPhysicalToCached(): illegal address.");
    return (void*)(paddr + 0x80000000);
}

// UNUSED: the game never calls this; dead-stripped at link.
void * OSPhysicalToUncached(u32 paddr) {
    ASSERTMSGLINE(0x3B, paddr < 0x10000000U, "OSPhysicalToUncached(): illegal address.");
    return (void*)(paddr - 0x40000000);
}

// UNUSED: the game never calls this; dead-stripped at link.
u32 OSCachedToPhysical(void * caddr) {
    ASSERTMSGLINE(0x4A, 0x80000000U <= (u32)caddr && (u32)caddr < 0x90000000U, "OSCachedToPhysical(): illegal address.");
    return (u32)caddr + 0x80000000;
}

// UNUSED: the game never calls this; dead-stripped at link.
u32 OSUncachedToPhysical(void * ucaddr) {
    ASSERTMSGLINE(0x59, 0xC0000000U <= (u32)ucaddr && (u32)ucaddr < 0xD0000000U, "OSUncachedToPhysical(): illegal address.");
    return (u32)ucaddr + 0x40000000;
}

// UNUSED: the game never calls this; dead-stripped at link.
void * OSCachedToUncached(void * caddr) {
    ASSERTMSGLINE(0x68, 0x80000000U <= (u32)caddr && (u32)caddr < 0x90000000U, "OSCachedToUncached(): illegal address.");
    return (void*)((u32)caddr + 0x40000000);
}

// UNUSED: the game never calls this; dead-stripped at link.
void * OSUncachedToCached(void * ucaddr) {
    ASSERTMSGLINE(0x77, 0xC0000000U <= (u32)ucaddr && (u32)ucaddr < 0xD0000000U, "OSUncachedToCached(): illegal address.");
    return (void*)((u32)ucaddr - 0x40000000);
}

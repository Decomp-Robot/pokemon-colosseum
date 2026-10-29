#include <dolphin.h>
#include <dolphin/os.h>

#include "__os.h"

// UNUSED: the game never calls this; dead-stripped at link.
long long __get_clock(void) {
    return -1;
}

// UNUSED: the game never calls this; dead-stripped at link.
unsigned long __get_time(void) {
    return (u32)OSTicksToSeconds(OSGetTime()) - 0x43E83E00;
}

// UNUSED: the game never calls this; dead-stripped at link.
int __to_gm_time() {
    return 0;
}

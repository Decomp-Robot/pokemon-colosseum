#include <dolphin.h>
#include <dolphin/ax.h>

#include "__ax.h"

// UNUSED: the game never calls this; dead-stripped at link.
void AXInit(void) {
#ifdef DEBUG
    OSReport("Initializing AX\n");
#endif
    __AXAllocInit();
    __AXVPBInit();
    __AXSPBInit();
    __AXAuxInit();
    __AXClInit();
    __AXOutInit();
}

// UNUSED: the game never calls this; dead-stripped at link.

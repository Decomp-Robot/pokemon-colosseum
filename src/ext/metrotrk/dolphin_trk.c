#include <dolphin/os/OSReset.h>

__declspec(section ".init") void __TRK_reset(void) {
    OSResetSystem(OS_RESET_RESTART, 0, FALSE);
}

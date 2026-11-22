#ifndef _DOLPHIN_OSFASTCAST_H_
#define _DOLPHIN_OSFASTCAST_H_

#include <dolphin/base/PPCArch.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void OSInitFastCast(void)
{
#ifdef __MWERKS__
    asm {
        li      r3, 0x0004
        oris    r3, r3, 0x0004
        mtspr   GQR2, r3
        li      r3, 0x0005
        oris    r3, r3, 0x0005
        mtspr   GQR3, r3
        li      r3, 0x0006
        oris    r3, r3, 0x0006
        mtspr   GQR4, r3
        li      r3, 0x0007
        oris    r3, r3, 0x0007
        mtspr   GQR5, r3
    }
#endif
}

#ifdef __cplusplus
}
#endif

#endif

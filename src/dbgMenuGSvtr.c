/*
 * Debug menu commands for the GSvtr video playback object.
 *
 * The four playback commands change the player's mode through the shared
 * video-player state. The display command refreshes that state from the
 * shared menu flag, while the active-object command starts or stops the player
 * according to the currently selected object.
 */
#include "pokemon/types.h"

/* Playback state shared with the GSgfx debug menu. */
extern u8 lbl_8047A298; /* playback active */

/* Shared video-player state. */
extern void zz_801E1170_(void);
extern void zz_801E118C_(void);
extern void zz_801E119C_(void);
extern void zz_801E11B0_(void);
extern u8 zz_801E11CC_(void);
extern void zz_801E11D4_(u8 displayState, u8 playState);
extern u8 zz_801E11E8_(void);
extern void zz_801E11F0_(void);
extern void zz_801E1258_(void);

/* Slow down video playback. */
int dbgMenuGSvtrSlow(void)
{
    zz_801E1170_();
    return 0;
}

/* Advance video playback by one step. */
int dbgMenuGSvtrStep(void)
{
    zz_801E118C_();
    return 0;
}

/* Pause video playback. */
int dbgMenuGSvtrPause(void)
{
    zz_801E11B0_();
    return 0;
}

/* Resume video playback. */
int dbgMenuGSvtrPlay(void)
{
    zz_801E119C_();
    return 0;
}

/* Update the video player's display mode from the shared playback state. */
int dbgMenuGSvtrChangeDisp(void)
{
    zz_801E11D4_(!zz_801E11CC_(), lbl_8047A298);
    return 0;
}

/* Start the player on the active object, or stop it otherwise. */
int dbgMenuGSvtrChangeActive(void)
{
    if (zz_801E11E8_() == 0) {
        zz_801E1258_();
    } else {
        zz_801E11F0_();
    }
    return 0;
}

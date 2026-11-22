/*
 * Debug menu commands for the GSgfx graphics-object display.
 *
 * The display is opened through the shared menu panel manager and its
 * playback is driven by two one-byte flags kept in shared runtime state:
 * one records whether playback is active, the other selects the step to
 * play when playback is paused. Playback commands stop a running sequence
 * before handing the updated state back to the graphics object.
 */
#include "pokemon/types.h"

#define GSGFX_MENU_INDEX 8
#define GSGFX_PANEL_POS_X 380
#define GSGFX_PANEL_POS_Y 32

/* Values for the GSgfx playback step selector. */
#define GSGFX_STEP_MODE_0 0
#define GSGFX_STEP_MODE_1 1

/* Playback state shared with the GSvtr debug menu. */
extern u8 lbl_8047A298; /* playback active */
extern u8 lbl_8047A299; /* paused step selector */

extern u8 zz_80102620_(int menuIndex);
extern void zz_80102510_(int menuIndex);
extern int zz_801026A4_(int menuIndex, ...);
extern void zz_80102868_(int menuIndex, int posX, int posY);

extern u8 zz_80175FFC_(void);
extern void zz_80176004_(void);
extern void zz_80176030_(int step);

extern int zz_801E11CC_(void);
extern void zz_801E11D4_(int displayState, u8 playState);

int dbgMenuGSgfxChangeDisp(void)
{
    if (zz_80102620_(GSGFX_MENU_INDEX) != 0) {
        zz_80102510_(GSGFX_MENU_INDEX);
    } else {
        zz_801026A4_(GSGFX_MENU_INDEX, 0, 0, 0, 1, 0);
        zz_80102868_(GSGFX_MENU_INDEX, GSGFX_PANEL_POS_X, GSGFX_PANEL_POS_Y);
    }
    return 0;
}

/* With playback enabled, stop the running player or start from the selected step. */
void zz_8000C0DC_(void)
{
    if (lbl_8047A298 != 0) {
        if (zz_80175FFC_() == 1) {
            zz_80176004_();
        } else if (lbl_8047A299 == GSGFX_STEP_MODE_0) {
            zz_80176030_(GSGFX_STEP_MODE_0);
        } else if (lbl_8047A299 == GSGFX_STEP_MODE_1) {
            zz_80176030_(GSGFX_STEP_MODE_1);
        }
    }
}

/* Toggle GSgfx playback, selecting step mode 1, then refresh the object. */
int zz_8000C144_(void)
{
    if (lbl_8047A298 == 0) {
        lbl_8047A299 = GSGFX_STEP_MODE_1;
        lbl_8047A298 = 1;
    } else {
        lbl_8047A298 = 0;
        if (zz_80175FFC_() == 1) {
            zz_80176004_();
        }
    }
    zz_801E11D4_(zz_801E11CC_(), lbl_8047A298);
    return 0;
}

/* Toggle GSgfx playback, selecting step mode 0, then refresh the object. */
int zz_8000C1A8_(void)
{
    if (lbl_8047A298 == 0) {
        lbl_8047A299 = GSGFX_STEP_MODE_0;
        lbl_8047A298 = 1;
    } else {
        lbl_8047A298 = 0;
        if (zz_80175FFC_() == 1) {
            zz_80176004_();
        }
    }
    zz_801E11D4_(zz_801E11CC_(), lbl_8047A298);
    return 0;
}

#ifndef PAD_STATE_H
#define PAD_STATE_H

#include "pokemon/types.h"

/*
 * Buttons of the controllers in one record, one bit per button and word. The
 * pad code keeps a record per port and merges the ports it is asked for into a
 * second record of the same shape, which is the one the menu code reads.
 */
typedef struct PadState {
    u16 held;           /* buttons that are down */
    u16 previousHeld;   /* buttons that were down in the previous frame */
    u16 pressed;        /* buttons that went down in this frame */
    u16 repeat;         /* pressed, together with the pulses of the buttons that stay down */
    u16 repeating;      /* buttons the pad is repeating */
    s8 repeatDelay[16]; /* frames left before each button repeats */
} PadState;

#endif

/*
 * Message-control value slots.
 *
 * A message control owns four 32-bit values that message scripts read back
 * through placeholders.  Set one of them; a missing control or an out-of-range
 * slot is ignored.
 */
#include "pokemon/types.h"

enum {
    MSGCTRL_VALUE_COUNT = 4,
    MSGCTRL_VALUES_OFFSET = 0x24,
};

typedef struct MsgCtrl {
    u8 reserved[MSGCTRL_VALUES_OFFSET]; /* Fields this setter does not touch. */
    u32 values[MSGCTRL_VALUE_COUNT];
} MsgCtrl;

void msgctrlInitValue(MsgCtrl *msgCtrl, u8 index, u32 value)
{
    if (msgCtrl == 0) {
        return;
    }
    if (index >= MSGCTRL_VALUE_COUNT) {
        return;
    }
    msgCtrl->values[index] = value;
}

/*
 * Debug menu commands for memory-card operations.
 *
 * Delete and save only start the card operation; load additionally displays
 * the result message and invokes the menu's follow-up routine.
 */
#include "app/message/message.h"

enum MemcardOperation {
    MEMCARD_OP_LOAD = 2,
    MEMCARD_OP_SAVE = 11,
    MEMCARD_OP_DELETE = 12,
};

enum {
    MSG_MEMCARD_LOAD = 6055,
};

extern int zz_801D0748_(int operation, int, int);

int dbgMenuMemcardDelete(void)
{
    zz_801D0748_(MEMCARD_OP_DELETE, 2, 0);
    return 0;
}

int dbgMenuMemcardLoad(void)
{
    zz_801D0748_(MEMCARD_OP_LOAD, 2, 0);
    displayMessage(MESSAGE_DISPLAY_KIND_MENU, MSG_MEMCARD_LOAD,
                   MESSAGE_DISPLAY_ARGUMENT_SET, MESSAGE_DISPLAY_ARGUMENT_SET);
    closeMessageWindow(MESSAGE_WINDOW_CLOSE_WAIT);
    return 0;
}

int dbgMenuMemcardSave(void)
{
    zz_801D0748_(MEMCARD_OP_SAVE, 2, 0);
    return 0;
}

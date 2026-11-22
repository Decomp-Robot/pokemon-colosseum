/*
 * Debug menu commands for memory-card operations.
 *
 * Delete and save only start the card operation; load additionally displays
 * the result message and invokes the menu's follow-up routine.
 */

enum MemcardOperation {
    MEMCARD_OP_LOAD = 2,
    MEMCARD_OP_SAVE = 11,
    MEMCARD_OP_DELETE = 12,
};

enum {
    MSG_MEMCARD_LOAD = 6055,
};

extern int zz_801D0748_(int operation, int, int);
extern int zz_display_message_with_msg_id(int, int messageId, int, int);
extern void zz_801069FC_(int);

int dbgMenuMemcardDelete(void)
{
    zz_801D0748_(MEMCARD_OP_DELETE, 2, 0);
    return 0;
}

int dbgMenuMemcardLoad(void)
{
    zz_801D0748_(MEMCARD_OP_LOAD, 2, 0);
    zz_display_message_with_msg_id(2, MSG_MEMCARD_LOAD, 1, 1);
    zz_801069FC_(1);
    return 0;
}

int dbgMenuMemcardSave(void)
{
    zz_801D0748_(MEMCARD_OP_SAVE, 2, 0);
    return 0;
}

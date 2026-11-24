/*
 * Debug menu commands for testing message windows.
 *
 * Commands 0 through 3 exercise the screen fade, a fixed test message and
 * the message variable slots. The check handler prompts for a message ID
 * while the R trigger is held and previews the entered message.
 */
#include "app/menu/menuPokemon.h"
#include "app/message/message.h"
#include "app/pad/padInput.h"
#include "app/task/task.h"
#include "pokemon/types.h"

#define TEST_MESSAGE_ID 17605
#define MESSAGE_VARIABLE_INDEX 49
#define MESSAGE_VARIABLE_VALUE 2010
#define PAD_TRIGGER_R 0x20

extern void zz_801C41C8_(int mode, float seconds);
extern void zz_801C40F0_(int wait);
extern void zz_80105FF8_(int msgId, int unused, int wait);
extern void zz_801069FC_(int wait);
extern void zz_80106080_(int wait);
extern void zz_80105FB0_(int dummy);
extern void zz_801067E8_(int msgId, int wait, int flag);
extern void zz_8001D7E4_(void);
extern void zz_801065B8_(int wait);

/* Fade timing literals in seconds, shared by both fade directions. */
const float dbgMenuMsgFadeTimes[2] = { 0.5f, 0.0f };

int dbgMenuMsgTest(int menuItem, int command)
{
    switch (command) {
    case 0:
        zz_801C41C8_(3, dbgMenuMsgFadeTimes[0]);
        zz_801C40F0_(1);
        zz_80105FF8_(TEST_MESSAGE_ID, 1, 0);
        zz_801069FC_(1);
        zz_801C41C8_(2, dbgMenuMsgFadeTimes[0]);
        zz_801C40F0_(1);
        break;
    case 1:
        zz_80106080_(1);
        zz_80105FB0_(1);
        break;
    case 2:
        waitFrame();
        waitFrame();
        waitFrame();
        zz_80106080_(0);
        waitFrame();
        waitFrame();
        break;
    case 3:
        zz_set_msg_var(MESSAGE_VARIABLE_INDEX, MESSAGE_VARIABLE_VALUE);
        break;
    }

    return 0;
}

int dbgMenuMsgCheck(void)
{
    u32 msgId;

    if ((padGetButtons(1) & PAD_TRIGGER_R) != 0) {
        if (menuInputNumber(0, &msgId) == 0) {
            return -1;
        }
        zz_801067E8_(msgId, 1, 0);
        zz_8001D7E4_();
        zz_801065B8_(1);
        return 0;
    }
    return 1;
}

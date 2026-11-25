#ifndef APP_MSGCTRL_MSGCTRL_H
#define APP_MSGCTRL_MSGCTRL_H

#include "pokemon/types.h"

#define MSGCTRL_PARAM_COUNT 8
#define MSGCTRL_VALUE_COUNT 4

typedef struct MsgCtrl {
    u8 mode;
    u8 pad;
    u16 nameId;
    u16 trainerPokemonIndex;
    u16 kind;
    u32 messageId;
    u32 messageKind;
    u32 messageSubId;
    u16 params[MSGCTRL_PARAM_COUNT];
    u32 value[MSGCTRL_VALUE_COUNT];
} MsgCtrl;

void msgctrlInitValue(MsgCtrl *msgCtrl, u8 index, u32 value);
void msgctrlSetMessageKind(MsgCtrl *msgCtrl, u32 value);
void msgctrlSetParam(MsgCtrl *msgCtrl, u8 index, u16 value);
void msgctrlSetTrainerPokemonIndex(MsgCtrl *msgCtrl, u16 value);
void msgctrlSetMessageId(MsgCtrl *msgCtrl, u32 value);
void msgctrlSetKind(MsgCtrl *msgCtrl, u16 value);
void msgctrlSetMode(MsgCtrl *msgCtrl, u8 value);
u32 msgctrlGetMessageSubId(const MsgCtrl *msgCtrl);
u32 msgctrlGetValue(const MsgCtrl *msgCtrl, u8 index);
u32 msgctrlGetMessageKind(const MsgCtrl *msgCtrl);
u16 msgctrlGetParam(const MsgCtrl *msgCtrl, u8 index);
u16 msgctrlGetTrainerPokemonIndex(const MsgCtrl *msgCtrl);
void msgctrlSetNameId(MsgCtrl *msgCtrl, u16 value);
u16 msgctrlGetNameId(const MsgCtrl *msgCtrl);
u32 msgctrlGetMessageId(const MsgCtrl *msgCtrl);
u16 msgctrlGetKind(const MsgCtrl *msgCtrl);
u8 msgctrlGetMode(const MsgCtrl *msgCtrl);

#endif

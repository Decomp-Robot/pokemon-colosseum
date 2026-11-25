#include "app/msgctrl/msgctrl.h"

void msgctrlInitValue(MsgCtrl *msgCtrl, u8 index, u32 value)
{
    if (msgCtrl == 0) {
        return;
    }
    if (index >= MSGCTRL_VALUE_COUNT) {
        return;
    }
    msgCtrl->value[index] = value;
}

void msgctrlSetMessageKind(MsgCtrl *msgCtrl, u32 value)
{
    if (msgCtrl == 0) {
        return;
    }
    msgCtrl->messageKind = value;
}

void msgctrlSetParam(MsgCtrl *msgCtrl, u8 index, u16 value)
{
    if (msgCtrl == 0) {
        return;
    }
    if (index >= MSGCTRL_PARAM_COUNT) {
        return;
    }
    msgCtrl->params[index] = value;
}

void msgctrlSetTrainerPokemonIndex(MsgCtrl *msgCtrl, u16 value)
{
    if (msgCtrl == 0) {
        return;
    }
    msgCtrl->trainerPokemonIndex = value;
}

void msgctrlSetMessageId(MsgCtrl *msgCtrl, u32 value)
{
    if (msgCtrl == 0) {
        return;
    }
    msgCtrl->messageId = value;
}

void msgctrlSetKind(MsgCtrl *msgCtrl, u16 value)
{
    if (msgCtrl == 0) {
        return;
    }
    msgCtrl->kind = value;
}

void msgctrlSetMode(MsgCtrl *msgCtrl, u8 value)
{
    if (msgCtrl == 0) {
        return;
    }
    msgCtrl->mode = value;
}

u32 msgctrlGetMessageSubId(const MsgCtrl *msgCtrl)
{
    if (msgCtrl == 0) {
        return 0;
    }
    return msgCtrl->messageSubId;
}

u32 msgctrlGetValue(const MsgCtrl *msgCtrl, u8 index)
{
    if (msgCtrl == 0) {
        return 0;
    }
    if (index >= MSGCTRL_VALUE_COUNT) {
        return 0;
    }
    return msgCtrl->value[index];
}

u32 msgctrlGetMessageKind(const MsgCtrl *msgCtrl)
{
    if (msgCtrl == 0) {
        return 0;
    }
    return msgCtrl->messageKind;
}

u16 msgctrlGetParam(const MsgCtrl *msgCtrl, u8 index)
{
    if (msgCtrl == 0) {
        return 0;
    }
    if (index >= MSGCTRL_PARAM_COUNT) {
        return 0;
    }
    return msgCtrl->params[index];
}

u16 msgctrlGetTrainerPokemonIndex(const MsgCtrl *msgCtrl)
{
    if (msgCtrl == 0) {
        return 0;
    }
    return msgCtrl->trainerPokemonIndex;
}

void msgctrlSetNameId(MsgCtrl *msgCtrl, u16 value)
{
    if (msgCtrl == 0) {
        return;
    }
    msgCtrl->nameId = value;
}

u16 msgctrlGetNameId(const MsgCtrl *msgCtrl)
{
    if (msgCtrl == 0) {
        return 0;
    }
    return msgCtrl->nameId;
}

u32 msgctrlGetMessageId(const MsgCtrl *msgCtrl)
{
    if (msgCtrl == 0) {
        return 0;
    }
    return msgCtrl->messageId;
}

u16 msgctrlGetKind(const MsgCtrl *msgCtrl)
{
    if (msgCtrl == 0) {
        return 0;
    }
    return msgCtrl->kind;
}

u8 msgctrlGetMode(const MsgCtrl *msgCtrl)
{
    if (msgCtrl == 0) {
        return 0;
    }
    return msgCtrl->mode;
}

#ifndef APP_FLOOR_FLOOREVENT_H
#define APP_FLOOR_FLOOREVENT_H

#include "pokemon/types.h"

enum SoundId {
    SOUND_ERROR = 38,
};

int soundPlay(u32 soundId);

#ifdef __cplusplus
extern "C" {
#endif

void soundPushVolume(int volume, int fadeTime, int se, int bgm);
void soundPopVolume(int fadeTime, int se, int bgm);

#ifdef __cplusplus
}
#endif

#endif

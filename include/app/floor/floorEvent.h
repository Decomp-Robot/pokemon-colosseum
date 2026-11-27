#ifndef APP_FLOOR_FLOOREVENT_H
#define APP_FLOOR_FLOOREVENT_H

#include "pokemon/types.h"

enum SoundId {
    SOUND_ERROR = 38,
};

int soundPlay(u32 soundId);

#define SOUND_VOLUME_STACK_SIZE 3

typedef struct SoundVolumeStack {
    u8 reserved0;
    u8 currentVolume;
    u8 reserved1[2];
    u8 count;
    u8 volumes[SOUND_VOLUME_STACK_SIZE];
} SoundVolumeStack;

typedef struct SoundChannel {
    u8 flags;
    u8 reserved[7];
    SoundVolumeStack *volumeStack;
} SoundChannel;

#ifdef __cplusplus
extern "C" {
#endif

void soundPushVolume(int volume, int fadeTime, u32 se, u32 bgm);
void soundPopVolume(int fadeTime, u32 se, u32 bgm);
int popChannelVolume(u32 channelIndex, int fadeTime);
int pushChannelVolume(u32 channelIndex, int fadeTime, int volume);

extern SoundChannel *soundChannels;
extern u32 soundChannelCount;
int applyChannelVolume(u32 channelIndex, int fadeTime, int volume);

#ifdef __cplusplus
}
#endif

#endif

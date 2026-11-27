#include "app/floor/floorEvent.h"

enum SoundChannelFlagBit {
    SOUND_CHANNEL_ACTIVE_BIT = 5,
    SOUND_CHANNEL_BGM_BIT = 7,
};

void soundPopVolume(int fadeTime, u32 se, u32 bgm)
{
    int i;
    u32 flags;

    for (i = 0; i < soundChannelCount; i++) {
        flags = soundChannels[i].flags;
        if (((flags >> SOUND_CHANNEL_ACTIVE_BIT) & 1) != 1) {
            continue;
        }
        if (((flags >> SOUND_CHANNEL_BGM_BIT) & 1) == 1) {
            if (bgm == 0) {
                continue;
            }
        } else if (se == 0) {
            continue;
        }
        popChannelVolume(i, fadeTime);
    }
}

int popChannelVolume(u32 channelIndex, int fadeTime)
{
    SoundChannel *channel = &soundChannels[channelIndex];
    SoundVolumeStack *stack = channel->volumeStack;

    if (stack == 0) {
        return 0;
    }
    if (stack->count == 0) {
        return 0;
    }
    stack->count--;
    return applyChannelVolume(channelIndex, fadeTime, stack->volumes[stack->count]);
}

void soundPushVolume(int volume, int fadeTime, u32 se, u32 bgm)
{
    int i;
    u32 flags;

    for (i = 0; i < soundChannelCount; i++) {
        flags = soundChannels[i].flags;
        if (((flags >> SOUND_CHANNEL_ACTIVE_BIT) & 1) != 1) {
            continue;
        }
        if (((flags >> SOUND_CHANNEL_BGM_BIT) & 1) == 1) {
            if (bgm == 0) {
                continue;
            }
        } else if (se == 0) {
            continue;
        }
        pushChannelVolume(i, fadeTime, volume);
    }
}

int pushChannelVolume(u32 channelIndex, int fadeTime, int volume)
{
    SoundChannel *channel = &soundChannels[channelIndex];
    SoundVolumeStack *stack = channel->volumeStack;
    int currentVolume;
    u8 count;

    if (stack == 0) {
        return 0;
    }
    count = stack->count;
    if (count >= SOUND_VOLUME_STACK_SIZE) {
        return 0;
    }
    currentVolume = stack->currentVolume;
    if (currentVolume < volume) {
        volume = currentVolume;
    }
    stack->volumes[count] = currentVolume;
    if (stack->count < SOUND_VOLUME_STACK_SIZE - 1) {
        stack->count++;
    }
    return applyChannelVolume(channelIndex, fadeTime, volume);
}

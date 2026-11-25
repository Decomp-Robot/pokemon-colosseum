/*
 * Debug menu commands for the sound test.
 *
 * Two handlers change the SE and BGM master volumes. The remaining handlers
 * step through the sound lists: a select window supplies an index into a
 * count/table pair, and the selected sound is played or stopped. The
 * environment lists first stop every other sound in the same list so that
 * only one ambient track is active at a time.
 */
#include "app/menu/menuWindow.h"
#include "pokemon/types.h"

enum DbgMenuCommand {
    DBG_MENU_SOUND_BGM_ON = 209,
    DBG_MENU_SOUND_BGM_OFF = 210,
    DBG_MENU_SOUND_SE_ON = 211,
    DBG_MENU_SOUND_SE_OFF = 212,
};

enum DbgMenuResult {
    DBG_MENU_STAY_OPEN = 0,
    DBG_MENU_CLOSE = -1,
};

enum SoundVolume {
    SOUND_VOLUME_OFF = 0,
    SOUND_VOLUME_MAX = 127,
};

enum SoundStatus {
    SOUND_STATUS_STOPPED = 0,
    SOUND_STATUS_STARTING = 1,
    SOUND_STATUS_PLAYING = 2,
    SOUND_STATUS_STOPPING = 3,
    SOUND_STATUS_PLAYING_END = 4,
};

extern void zz_80166D18_(u8 volume, u16 time, u8 music, u8 fx);
extern int zz_801666BC_(u16 soundId);
extern void zz_80166B18_(u16 soundId);
extern void zz_801659FC_(u16 soundId, int fade, u8 volume);
extern void zz_80165A20_(u16 soundId, int fade, u8 volume);

u16 *dbgMenuSoundEnvBgmTable;
u32 *dbgMenuSoundEnvBgmCount;
u16 *dbgMenuSoundEnvSeTable;
u32 *dbgMenuSoundEnvSeCount;
u16 *dbgMenuSoundBgmTable;
u32 *dbgMenuSoundBgmCount;
u16 *dbgMenuSoundSeTable;
u32 *dbgMenuSoundSeCount;

int dbgMenuSoundTestChangeSE(int command)
{
    switch (command) {
    case DBG_MENU_SOUND_SE_ON:
        zz_80166D18_(SOUND_VOLUME_MAX, 0, 0, 1);
        break;
    case DBG_MENU_SOUND_SE_OFF:
        zz_80166D18_(SOUND_VOLUME_OFF, 0, 0, 1);
        break;
    }
    return DBG_MENU_STAY_OPEN;
}

int dbgMenuSoundTestChangeBGM(int command)
{
    switch (command) {
    case DBG_MENU_SOUND_BGM_ON:
        zz_80166D18_(SOUND_VOLUME_MAX, 0, 1, 0);
        break;
    case DBG_MENU_SOUND_BGM_OFF:
        zz_80166D18_(SOUND_VOLUME_OFF, 0, 1, 0);
        break;
    }
    return DBG_MENU_STAY_OPEN;
}

/* Play the chosen entry as a sound effect, or stop it when its status says otherwise. */
static inline void dbgMenuSoundTestPlayOrStopSE(u16 soundId)
{
    int status = zz_801666BC_(soundId);

    if (status != SOUND_STATUS_STOPPED && status >= SOUND_STATUS_STOPPED && status < SOUND_STATUS_PLAYING_END) {
        zz_80166B18_(soundId);
    } else {
        zz_801659FC_(soundId, 0, SOUND_VOLUME_MAX);
    }
}

/* BGM list selection: the entry is played as a sound effect. */
int zz_8000C6EC_(void)
{
    int index;

    while ((index = menuWindowOpen(MENU_WINDOW_DEBUG_INPUT, 1)) != DBG_MENU_CLOSE) {
        if (index >= *dbgMenuSoundBgmCount) {
            continue;
        }
        dbgMenuSoundTestPlayOrStopSE(dbgMenuSoundBgmTable[index]);
    }
    menuWindowCloseSingle(MENU_WINDOW_DEBUG_INPUT);
    return DBG_MENU_CLOSE;
}

/* SE list selection. */
int zz_8000C788_(void)
{
    int index;

    while ((index = menuWindowOpen(MENU_WINDOW_DEBUG_INPUT, 1)) != DBG_MENU_CLOSE) {
        if (index >= *dbgMenuSoundSeCount) {
            continue;
        }
        dbgMenuSoundTestPlayOrStopSE(dbgMenuSoundSeTable[index]);
    }
    menuWindowCloseSingle(MENU_WINDOW_DEBUG_INPUT);
    return DBG_MENU_CLOSE;
}

/* Environment sound list: stop the other entries before toggling the choice. */
int dbgMenuSoundTestENV(void)
{
    int index;

    while ((index = menuWindowOpen(MENU_WINDOW_DEBUG_INPUT, 1)) != DBG_MENU_CLOSE) {
        int other;
        int status;

        if (index >= *dbgMenuSoundEnvSeCount) {
            continue;
        }
        for (other = 0; other < *dbgMenuSoundEnvSeCount; other++) {
            if (index == other) {
                continue;
            }
            status = zz_801666BC_(dbgMenuSoundEnvSeTable[other]);
            switch (status) {
            case SOUND_STATUS_STARTING:
            case SOUND_STATUS_PLAYING:
            case SOUND_STATUS_STOPPING:
                zz_80166B18_(dbgMenuSoundEnvSeTable[other]);
                break;
            }
        }
        dbgMenuSoundTestPlayOrStopSE(dbgMenuSoundEnvSeTable[index]);
    }
    menuWindowCloseSingle(MENU_WINDOW_DEBUG_INPUT);
    return DBG_MENU_CLOSE;
}

/* Play the chosen entry as background music, or stop it when its status says otherwise. */
static inline void dbgMenuSoundTestPlayOrStopBGM(u16 soundId)
{
    int status = zz_801666BC_(soundId);

    if (status != SOUND_STATUS_STOPPED && status >= SOUND_STATUS_STOPPED && status < SOUND_STATUS_PLAYING_END) {
        zz_80166B18_(soundId);
    } else {
        zz_80165A20_(soundId, 0, SOUND_VOLUME_MAX);
    }
}

/* Environment BGM list. */
int zz_8000C92C_(void)
{
    int index;

    while ((index = menuWindowOpen(MENU_WINDOW_DEBUG_INPUT, 1)) != DBG_MENU_CLOSE) {
        int other;
        int status;

        if (index >= *dbgMenuSoundEnvBgmCount) {
            continue;
        }
        for (other = 0; other < *dbgMenuSoundEnvBgmCount; other++) {
            if (index == other) {
                continue;
            }
            status = zz_801666BC_(dbgMenuSoundEnvBgmTable[other]);
            switch (status) {
            case SOUND_STATUS_STARTING:
            case SOUND_STATUS_PLAYING:
            case SOUND_STATUS_STOPPING:
                zz_80166B18_(dbgMenuSoundEnvBgmTable[other]);
                break;
            }
        }
        dbgMenuSoundTestPlayOrStopBGM(dbgMenuSoundEnvBgmTable[index]);
    }
    menuWindowCloseSingle(MENU_WINDOW_DEBUG_INPUT);
    return DBG_MENU_CLOSE;
}

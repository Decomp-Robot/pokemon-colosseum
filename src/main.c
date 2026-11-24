/*
 * Game startup and the callbacks registered by it.
 *
 * main() clears the low memory block, builds the game heap from the OS arena
 * and starts the AR, ARQ and AI subsystems. runGame() then configures the
 * remaining engine subsystems, installs the reset and per-frame callbacks,
 * creates the game-initialisation thread and runs the frame loop. The
 * remaining functions are those callbacks and the clock and reset helpers
 * they use.
 */
#include "pokemon/types.h"
#include <dolphin/ar.h>
#include <dolphin/ai.h>
#include <dolphin/os.h>
#include <dolphin/pad.h>
#include <dolphin/si.h>
#include <dolphin/vi.h>
#include <dolphin/mtx.h>
#include <libc/string.h>
#include "app/pad/padInput.h"

/* Opaque game thread object used by the play clock. */
typedef struct GSthread GSthread;

/* Callbacks and helpers defined in this file. */
void runGame(void);
void initGameThread(void);
void handleResetButton(void);
void clearMemoryBlocks(void);
void drawFrame(void);
void updateFrame(void);
void zz_80005FA8_(u32 arg0, u32 arg1);
void initPadPorts(void);
void resetPadPorts(void);
void initSoundListener(void);

/* AI stack buffer. */
#define AI_STACK_SIZE 0x4000
extern u8 lbl_8039A700[AI_STACK_SIZE];

/* AR queue state and the pointers read during startup. */
extern u32 lbl_80478DC0[2];
extern u32 *lbl_80478FB8;
extern u32 *lbl_80478FBC;

/* Reset and play-clock state shared with the callbacks. */
extern u8 lbl_80478DC8;  /* Nonzero while the play clock is running. */
extern u8 lbl_80478DC9;  /* Nonzero while the reset button is enabled. */
extern u8 lbl_80478DCA;  /* Set once the reset button has been pressed. */
extern s32 lbl_80478DCC; /* Countdown to the next memory clear. */

/* Bytes shared with the draw callback. */
extern u8 lbl_80478DD0[8];
extern u8 lbl_80478820[8];

/* Block bounds table used by the periodic memory clear. */
extern u32 lbl_802663A0[];

/* Format string used to report the available memory. */
extern char lbl_80266420[];

extern void zz_80006250_(void);
extern void zz_800E3568_(u32 arg0, void *start, void *end);
extern void zz_800E3560_(u32 arg0);
extern void zz_800DDF54_(u32 arg0, u32 arg1);
extern void zz_800EEDF8_(u32 arg0);
extern void zz_80167FA4_(int argc, char **argv, u32 arg2);
extern u32 OSGetProgressiveMode(void);
extern OSThread *OSSetIdleFunction(void (*idleFunction)(void *), void *param, void *stack,
                                   unsigned long stackSize);
extern GSthread *zz_80128E24_();
extern GSthread *zz_80128E04_();
extern u8 *zz_80128E2C_(void);
extern void zz_80128E14_(u8 *value);
extern void zz_80128E38_(u32 arg0, u32 arg1);
extern GSthread *GSthreadIsRunning();
extern f32 zz_80135C10_(GSthread *thread);
extern void zz_80135B5C_(GSthread *thread, f32 value);
extern void zz_800F09D8_(s32 arg0);
extern void zz_800D39E0_(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5);
extern void zz_800F80B0_(u32 port);
extern void zz_800F7E9C_(u32 port, u32 arg1);
extern void zz_800F7E40_(u32 port, u32 arg1);
extern void zz_800D37D4_(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5);
extern void zz_800EFFC0_(u32 arg0);
extern void zz_80191484_(u32 arg0);
extern void zz_800E4D3C_(u32 arg0);
extern void zz_800EE880_(u32 arg0);
extern void zz_800DF854_(u32 arg0);
extern void zz_800D2AD4_(u32 arg0);
extern void zz_800DD0B8_(u32 arg0);
extern void zz_80119824_(u32 arg0, u32 arg1);
extern void zz_80132C6C_(u32 arg0, u32 arg1, u32 arg2, u32 arg3);
extern void zz_800F8138_(void);
extern void zz_800F0448_(void);
extern u8 zz_8000DAA8_(void);
extern void zz_80181850_(u32 arg0, u32 arg1);
extern void zz_801E12A0_(void);
extern void zz_801E1274_(void);
extern void zz_8010C220_(void);
extern void zz_80179BEC_(void);
extern void zz_8013024C_(void);
extern void zz_8018E920_(u32 arg0);
extern void zz_801ED388_(void);
extern void zz_801D0A30_(void);
extern void zz_801128A0_(void);
extern void *zz_800F9544_(u32 arg0, u32 arg1, u32 arg2, u32 arg3);
extern void zz_800FF81C_(void *arg0, u32 arg1);
extern void *zz_trainer_get_data_pointer(u32 arg0, u32 index);
extern void zz_801909A8_(void *arg0, void *arg1, void *arg2, void *arg3, void *arg4, void *arg5);
extern void zz_800D361C_(u8 arg0);
extern void zz_800D30F0_(u32 arg0);
extern void zz_80101B90_(u32 arg0);
extern void zz_80101D8C_(void);
extern void zz_80101D5C_(void);
extern void zz_80175F6C_(void);
extern void zz_801E0FB4_(s32 arg0, u32 arg1, u32 arg2);
extern u32 zz_800F7AF0_(u32 port);
extern void zz_8000C0DC_(void);
extern u8 zz_801E11E8_(void);
extern s32 zz_801E11E0_(void);
extern void zz_801E11B0_(void);
extern void zz_801E119C_(void);
extern void zz_801E118C_(void);
extern void zz_801028B0_(void);
extern void zz_800FBF10_(void);
extern u32 zz_801906A0_(u32 id);
extern s32 zz_800D37CC_(void);
extern u32 zz_800D3088_(void);
extern void zz_8008AC34_(u32 port);
extern u8 zz_8008ABA0_(u32 port);
extern u8 zz_800F7EF8_(u32 port);
extern void zz_800F7F64_(u32 port);
extern void *zz_801664F0_(Vec *pos);
extern void zz_800F9378_(void *obj, u32 arg1, u32 arg2, u32 arg3);
extern s32 zz_800F7758_(u32 count);
extern void zz_800366A8_(void);
extern s32 zz_800FE834_(u32 type, u8 priority, u32 data, void (*callback)());
extern void zz_800FC528_(u32 arg0, u32 arg1);
extern void zz_800DD970_(char *format, ...);
extern u32 zz_800E0DDC_(void);
extern void zz_800F7DE4_(u32 port, u32 value);
extern void zz_800FE9B0_(u32 arg0, u32 arg1);
extern void zz_800F9670_(u32 arg0);
extern void zz_800FF828_(u32 a, u32 b, u32 c, u32 d);
extern void zz_8010D170_(void);
extern void zz_800FC518_(void *table);
extern void zz_80167DC0_(u32 a, u32 b, u32 c, u32 d, u32 e);
extern void zz_801E1300_(void);
extern void zz_801ED740_(void);
extern void zz_801E1B2C_(void);
extern void zz_80103CD8_(u32 arg0);
extern void zz_80101FB8_(u32 arg0);
extern void zz_800D3074_(u32 arg0);
extern void zz_801C47D0_(void);
extern void zz_80168638_(u32 arg0);
extern void zz_80130CE0_(u32 arg0);
extern void zz_Script_register_and_relocate(void *table);
extern void zz_800F07A8_(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, void (*callback)(void));
extern void zz_800FE7A0_(void);
extern void zz_80166E44_(void);
extern void *zz_8025DBD4_(u32 id);
extern void zz_Script_registry_insert(void *table);
extern void zz_800FC244_(void *table);
extern void zz_Script_set_builtin_function_table(void *table);
extern char lbl_8027A500[];
extern char lbl_802BD260[];
extern char lbl_802C0CB0[];
extern char lbl_802CF810[];
extern char lbl_802E1CF0[];
extern char lbl_80363778[];
extern void zz_801EF5C0_(void);

#define GAME_HEAP_SIZE 0xE80000 /* 14.5 MiB */

/* Low memory block cleared during startup. */
#define LOW_MEMORY_START ((u8 *)0x80001800)
#define LOW_MEMORY_END ((u8 *)0x80003000)
#define LOW_MEMORY_SIZE ((u32)LOW_MEMORY_END - (u32)LOW_MEMORY_START)

/* Offsets applied to the block bounds stored in the table. */
#define BLOCK_START_OFFSET 21
#define BLOCK_END_OFFSET 22

/* The play clock stops at 99:59:00. */
#define PLAY_CLOCK_MAX 359940.0f

/* Frames between periodic clears of the unused memory blocks. */
#define MEMORY_CLEAR_INTERVAL 100

/* Script flag that holds the play clock. */
#define PLAY_CLOCK_HOLD_FLAG 2222

/* Mask applied to the OS time to spread the initial script offset. */
#define TIME_SEED_MASK 0x7E0

/* Number of controller ports and the stack size for the game thread. */
#define PAD_PORT_COUNT 4
#define GAME_THREAD_STACK_SIZE 0x4000

/* Priorities used when the frame callbacks are registered. */
enum {
    CALLBACK_PRIORITY_PAD_RESET = 0,
    CALLBACK_PRIORITY_PAD_INIT = 1,
    CALLBACK_PRIORITY_CONTROLLER = 10,
    CALLBACK_PRIORITY_MEMORY_CLEAR = 253,
    CALLBACK_PRIORITY_FRAME_UPDATE = 254,
    CALLBACK_PRIORITY_END_OF_FRAME = 255
};

#define OS_RESETCODE_HOT 0x80000000
#define OSIsHotReset() ((OSGetResetCode() == OS_RESETCODE_HOT) ? TRUE : FALSE)

int main(int argc, char **argv)
{
    void *arenaLo;
    void *arenaEnd;

    memset(LOW_MEMORY_START, 0, LOW_MEMORY_SIZE);
    zz_80006250_();
    arenaLo = OSGetArenaLo();
    OSGetArenaHi();
    arenaEnd = (void *)((u8 *)arenaLo + GAME_HEAP_SIZE);
    zz_800E3568_(0, arenaLo, arenaEnd);
    zz_800E3560_(0);
    OSSetArenaLo(OSInitAlloc(arenaEnd, OSGetArenaHi(), 1));

    ARInit(lbl_80478DC0, 2);
    ARQInit();
    AIInit(lbl_8039A700 + AI_STACK_SIZE - 8);
    zz_800DDF54_(0x10000, 1);
    zz_800EEDF8_(0);
    zz_80167FA4_(argc, argv, 1);
    runGame();
}

u8 setResetEnabled(u8 value)
{
    u8 old = lbl_80478DC9;

    lbl_80478DC9 = value;
    return old;
}

u8 clearResetEnabled(void)
{
    u8 old = lbl_80478DC9;

    lbl_80478DC9 = FALSE;
    return old;
}

void setClockEnabled(u8 value)
{
    lbl_80478DC8 = value;
}

static inline GSthread *getPlayClockThread(void)
{
    if (zz_80128E24_() != NULL && zz_80128E04_() != NULL) {
        return GSthreadIsRunning();
    }
    return NULL;
}

void setPlayClock(f32 value)
{
    GSthread *thread = getPlayClockThread();

    if (thread != NULL) {
        zz_80135B5C_(thread, value);
    }
}

u32 getPlayClock(void)
{
    GSthread *thread = getPlayClockThread();
    f32 value;

    if (thread != NULL) {
        value = zz_80135C10_(thread);
    } else {
        value = 0.0f;
    }
    return value;
}

s32 zz_800057A0_(void) { return 1; }
s32 zz_800057A8_(void) { return 6; }

void runGame(void)
{
    int i;
    u8 progressiveReset;

    zz_800F09D8_(20);
    zz_800D39E0_(450000, 16, 8, 32, 1, 480);
    progressiveReset = FALSE;
    if (OSGetProgressiveMode() == 1 && OSIsHotReset() == TRUE) {
        progressiveReset = TRUE;
    }
    if (progressiveReset != FALSE) {
        zz_800D37D4_(1, 2, 0, 2, 1, 0);
    }
    zz_800EFFC0_(16);
    zz_80191484_(8);
    zz_800E4D3C_(64);
    zz_800EE880_(8);
    zz_800DF854_(1024);
    zz_800D2AD4_(32);
    zz_800DD0B8_(32);
    zz_80119824_(32, 128);
    zz_80132C6C_(16, 64, 1, 0x2000);
    zz_800F8138_();
    for (i = 1; i <= PAD_PORT_COUNT; i++) {
        zz_800F80B0_(i);
        zz_800F7E9C_(i, 2);
        zz_800F7E40_(i, 7);
        zz_800F7DE4_(i, 1);
    }
    zz_800FE9B0_(16, 4);
    zz_800F9670_(300);
    zz_800FF828_(4, 16, 16, 16);
    zz_8010D170_();
    zz_800F7758_(16);
    zz_Script_set_builtin_function_table(lbl_802E1CF0);
    zz_800FC528_(2, 5);
    zz_800FC518_(lbl_80363778);
    zz_Script_registry_insert(lbl_8027A500);
    zz_Script_registry_insert(lbl_802BD260);
    zz_Script_registry_insert(lbl_802C0CB0);
    zz_800FC244_(lbl_802CF810);
    zz_80167DC0_(15336, 15338, 15341, 15343, 15346);
    zz_801E1300_();
    zz_801ED740_();
    zz_801E1B2C_();
    zz_80103CD8_(24);
    zz_80101FB8_(4);
    zz_800D3074_(1);
    zz_800FE834_(1, CALLBACK_PRIORITY_END_OF_FRAME, 0, drawFrame);
    zz_800FE834_(1, CALLBACK_PRIORITY_PAD_RESET, 0, resetPadPorts);
    zz_800FE834_(1, CALLBACK_PRIORITY_PAD_INIT, 0, initPadPorts);
    zz_800FE834_(1, CALLBACK_PRIORITY_CONTROLLER, 0, zz_80005FA8_);
    zz_801E12A0_();
    zz_800FE834_(1, CALLBACK_PRIORITY_MEMORY_CLEAR, 0, clearMemoryBlocks);
    zz_800FE834_(1, CALLBACK_PRIORITY_FRAME_UPDATE, 0, updateFrame);
    zz_800FE834_(1, CALLBACK_PRIORITY_END_OF_FRAME, 0, handleResetButton);
    lbl_80478DC9 = TRUE;
    zz_801C47D0_();
    zz_80168638_(8);
    zz_80130CE0_(32);
    zz_800DD970_(lbl_80266420, zz_800E0DDC_());
    zz_800F07A8_(0, 1000, GAME_THREAD_STACK_SIZE, 1, 1, initGameThread);
    while (1) {
        zz_800FE7A0_();
    }
}

void initGameThread(void)
{
    u32 offset;
    u8 *buffer;

    zz_800366A8_();
    zz_800FF81C_(lbl_80478FBC, lbl_80478FB8[0]);
    buffer = zz_800F9544_(2, 0, 2, 0);
    buffer[0] = 0;
    buffer[1] = 0;
    lbl_80478DD0[0] = 0;
    setPlayClock(0.0f);
    lbl_80478DC8 = FALSE;
    zz_Script_registry_insert(zz_8025DBD4_(1));
    zz_800FC244_(zz_8025DBD4_(4));
    zz_Script_register_and_relocate(zz_8025DBD4_(7));
    offset = (u32)OSGetTime() & TIME_SEED_MASK;
    zz_80128E14_(zz_80128E2C_() + offset);
    zz_80128E38_(0, 0);
    zz_801EF5C0_();
    zz_801909A8_(zz_trainer_get_data_pointer(0, 4), zz_trainer_get_data_pointer(0, 7),
                 zz_trainer_get_data_pointer(0, 5), zz_trainer_get_data_pointer(0, 8),
                 zz_trainer_get_data_pointer(0, 6), zz_trainer_get_data_pointer(0, 9));
    zz_801E1274_();
    zz_8010C220_();
    initSoundListener();
    zz_80179BEC_();
    zz_8013024C_();
    zz_8018E920_(48);
    zz_801ED388_();
    zz_801D0A30_();
    zz_801128A0_();
    memset(LOW_MEMORY_START + 3, 0, LOW_MEMORY_SIZE - 3);
}

void handleResetButton(void)
{
    if (!lbl_80478DCA) {
        if (OSGetResetButtonState() == TRUE) {
            lbl_80478DCA = TRUE;
        }
    } else if (OSGetResetButtonState() == FALSE && lbl_80478DC9 == TRUE) {
        zz_80166E44_();
        PADRecalibrate(PAD_CHAN0_BIT | PAD_CHAN1_BIT | PAD_CHAN2_BIT | PAD_CHAN3_BIT);
        VISetBlack(TRUE);
        VIFlush();
        VISetPreRetraceCallback(NULL);
        VISetPostRetraceCallback(NULL);
        OSSetIdleFunction(NULL, NULL, NULL, 0);
        VIWaitForRetrace();
        OSResetSystem(OS_RESET_RESTART, 0, TRUE);
    }
}

void clearMemoryBlocks(void)
{
    u32 i;

    if (lbl_80478DCC == 0) {
        for (i = 0; i < 32; i += 2) {
            u32 start = lbl_802663A0[i] + BLOCK_START_OFFSET;
            u32 end = lbl_802663A0[i + 1] + BLOCK_END_OFFSET;

            memset(OSPhysicalToCached(start), 0, end - start);
        }
        memset(LOW_MEMORY_START + 1, 0, LOW_MEMORY_SIZE - 1);
        lbl_80478DCC = MEMORY_CLEAR_INTERVAL;
    }
    lbl_80478DCC--;
}

void drawFrame(void)
{
    zz_80101B90_(0xFF00);
    zz_800D361C_(lbl_80478DD0[0]);
    zz_80101D8C_();
    zz_800D30F0_(0);
    zz_80175F6C_();
    if (lbl_80478820[0] == 0) {
        zz_801E0FB4_(16, 1, 1);
    } else {
        zz_801E0FB4_(-1, 1, 1);
    }
    zz_80101B90_(0xFFFF00);
    zz_80101D5C_();
}

void updateFrame(void)
{
    GSthread *thread;
    f32 clock;

    zz_80101B90_(0xFF);
    if ((padGetButtons(1) & zz_800F7AF0_(1)) & PAD_TRIGGER_Z) {
        zz_8000C0DC_();
    }
    if (zz_801E11E8_() != 0) {
        if ((padGetButtons(1) & zz_800F7AF0_(1)) & PAD_TRIGGER_L) {
            if (zz_801E11E0_() == 2) {
                zz_801E11B0_();
            } else {
                zz_801E119C_();
            }
        }
        if ((padGetButtons(1) & zz_800F7AF0_(1)) & PAD_TRIGGER_R) {
            zz_801E118C_();
        }
    }
    zz_801028B0_();
    zz_800FBF10_();
    thread = getPlayClockThread();
    if (thread != NULL && lbl_80478DC8 == TRUE && zz_801906A0_(PLAY_CLOCK_HOLD_FLAG) == 0) {
        clock = zz_80135C10_(thread);
        clock += (f32)zz_800D3088_() / zz_800D37CC_();
        if (clock >= PLAY_CLOCK_MAX) {
            clock = PLAY_CLOCK_MAX;
        }
        zz_80135B5C_(thread, clock);
    }
}

/* Per-frame callback registered by runGame(). */
void zz_80005FA8_(u32 arg0, u32 arg1)
{
    zz_800F0448_();
    if (!zz_8000DAA8_()) {
        zz_80181850_(arg0, arg1);
    }
}

/* Controller callback registered at PAD_INIT priority. */
void initPadPorts(void)
{
    int port;

    for (port = 1; port <= PAD_PORT_COUNT; port++) {
        if (!zz_800F7EF8_(port)) {
            zz_8008AC34_(port);
        }
    }
}

/* Controller callback registered at PAD_RESET priority. */
void resetPadPorts(void)
{
    int port;

    for (port = 1; port <= PAD_PORT_COUNT; port++) {
        if (!zz_8008ABA0_(port)) {
            zz_800F7F64_(port);
        }
    }
}

/* Sets up the sound listener at the origin. */
void initSoundListener(void)
{
    Vec pos = { 0.0f, 0.0f, 0.0f };

    zz_800F9378_(zz_801664F0_(&pos), 0, 2000, 0);
}

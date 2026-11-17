#include "hw_pc_internal.h"
#include "musyx/hardware.h"

// UNUSED: static SDL_Mutex *mutex, SDL_AudioStream *stream, SDL_Thread *thread,
// SDL_AtomicInt running, paused; SDL device state of a backend the Dolphin
// archive does not contain.

// UNUSED: audioThread; the SDL worker that drains the stream from
// sndPCRender, which the Dolphin archive does not contain.

// UNUSED: sndPCOpenAudio; the SDL device opener of the PC backend, which the
// Dolphin archive does not contain.

// UNUSED: sndPCStartAudio; the SDL worker starter of the PC backend, which the
// Dolphin archive does not contain.

// UNUSED: sndPCStopAudio; the SDL teardown of the PC backend. Its only call
// sites, snd_init.c:199 and hardware.c:217, are `#if MUSY_TARGET ==
// MUSY_TARGET_PC` arms that the Dolphin build compiles out.

// UNUSED: sndPCPauseAudio; the SDL pause/resume hook of the PC backend, which
// has no call site outside it.

// UNUSED: hwInitIrq; the Dolphin archive takes this hook from hw_dolphin.c.

// UNUSED: hwExitIrq; the Dolphin archive takes this hook from hw_dolphin.c.

// UNUSED: hwEnableIrq; the Dolphin archive takes this hook from hw_dolphin.c.

// UNUSED: hwDisableIrq; the Dolphin archive takes this hook from hw_dolphin.c.

// UNUSED: hwIRQEnterCritical; the Dolphin archive takes this hook from
// hw_dolphin.c.

// UNUSED: hwIRQLeaveCritical; the Dolphin archive takes this hook from
// hw_dolphin.c.

#include "musyx/platform.h"
#if MUSY_TARGET == MUSY_TARGET_DOLPHIN
#include "musyx/debugger.h"
#include "musyx/synthdata.h"

// debugger.a member src/musyx/debugger/debugger.c, the runtime debugger front
// end.  Every definition it has - the ten file-scope statics and the seventeen
// functions marked below - sits inside `#if MUSY_TARGET == MUSY_TARGET_PC`
// (pristine :4-210), and musyx/debugger.h guards the matching declarations the
// same way, so the member has no content at all on the Dolphin target this
// project builds for.  "musyx/platform.h" comes first so MUSY_TARGET_PC and
// MUSY_TARGET_DOLPHIN have their real values before debugger.h's guard is
// evaluated; the reference text relies on the build command line for that,
// and with both names undefined the guard would read `0 == 0` and select the
// PC body.
// The object this member compiles to has no .text, .data or .bss and so no
// image content.  Its only allocated section is a 0x20-byte .sdata2 holding
// the four weak constants `_half$localstatic3$sqrtf__Ff`,
// `_three$localstatic4$sqrtf__Ff`, `_half$localstatic3$sqrt__Ff` and
// `_three$localstatic4$sqrt__Ff` (0.5 and 3.0 each, twice) that <math.h>'s
// `extern inline sqrtf` (src/ext/dolsdk2001/include/libc/math.h:30) and `sqrt`
// (line 53) declare; they arrive through synthdata.h -> hardware.h -> musyx.h:7.
// No declaration in this member uses them, nothing references them, mwld drops
// them and the image is unaffected - the same pair profile.c, hw_pc_vsample.c
// and the other non-emitting runtime.a members record.
// splits.txt therefore gives this debugger.a member a zero-length .text slot of
// its own, 0x801653BC..0x801653BC, which is where it lands in link order - after
// runtime.a's last member (StdReverb/reverb.c) and immediately before the
// auto_01_801653BC_text split.  It does not take the 0x801643D8 slot the eight
// non-emitting runtime.a members share.
// One marker per definition the reference text has:

// UNUSED: g_paused; the pause flag, taken atomically by SetPaused/IsPaused.

// UNUSED: g_step_frame; the single-step flag, PC builds only.

// UNUSED: g_frame_count; the frame counter behind GetFrameCount and Capture.

// UNUSED: g_macro_steps; the macro-step counter behind GetMacroStepCount.

// UNUSED: g_hooks; the MUSYX_DEBUG_HOOKS block copied by SetHooks.

// UNUSED: g_event_sequence; the event counter stamped by emit.

// UNUSED: g_bp_macro; per-breakpoint macro id, PC builds only.

// UNUSED: g_bp_opcode; per-breakpoint opcode, PC builds only.

// UNUSED: g_bp_voice; per-breakpoint voice index, PC builds only.

// UNUSED: g_bp_enabled; per-breakpoint enable flag, PC builds only.

// UNUSED: musyxDebuggerSetHooks; copies or clears g_hooks.

// UNUSED: musyxDebuggerSetPaused; stores the pause flag.

// UNUSED: musyxDebuggerIsPaused; loads the pause flag.

// UNUSED: musyxDebuggerStepFrame; sets the single-step flag and clears pause.

// UNUSED: musyxDebuggerConsumeStepFrame; exchanges the single-step flag.

// UNUSED: musyxDebuggerSetBreakpoint; fills or disables one breakpoint slot.

// UNUSED: musyxDebuggerClearBreakpoints; disables every breakpoint slot.

// UNUSED: musyxDebuggerGetFrameCount; returns g_frame_count.

// UNUSED: musyxDebuggerGetMacroStepCount; returns g_macro_steps.

// UNUSED: musyxDebuggerGetVoiceMacroCommands; decodes a voice's MSTEP macro
// into MUSYX_DEBUG_MACRO_COMMAND entries.

// UNUSED: emit; stamps the event sequence and calls the user's onEvent hook.

// UNUSED: musyxDebuggerRuntimeFrame; per-frame MUSYX_DEBUG_EVENT_FRAME event.

// UNUSED: musyxDebuggerRuntimeMacroStep; per-step event plus breakpoint test.

// UNUSED: musyxDebuggerRuntimeVoiceStart; MUSYX_DEBUG_EVENT_VOICE_START event.

// UNUSED: musyxDebuggerRuntimeVoiceStop; MUSYX_DEBUG_EVENT_VOICE_STOP event.

// UNUSED: musyxDebuggerRuntimeMessage; MUSYX_DEBUG_EVENT_MESSAGE event.

// UNUSED: musyxDebuggerCapture; fills a MUSYX_DEBUG_SNAPSHOT from the synth,
// sequencer and stream state.

#endif

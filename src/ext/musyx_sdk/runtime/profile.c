#include "musyx/platform.h"
#if MUSY_TARGET == MUSY_TARGET_DOLPHIN
#include "dolphin/base/PPCArch.h"
#include "musyx/musyx.h"
#include "musyx/voice.h"

#include "musyx/profile.h"

// runtime.a member src/musyx/runtime/profile.c, the Gekko performance-monitor
// profiling translation unit.  Its whole body sits inside `#ifdef
// MUSY_BUILD_DEBUG`: musyx/version.h:4-6 defines that name only when the
// CodeWarrior `_DEBUG` flag is set, and this project builds the archive
// release-style (-DNDEBUG=1, no _DEBUG in this object's command line), so the
// outer guard reduces the member to its five #includes.  Inside that guard the
// reference text has two scopes:
//   * sndProfUserCallback (pristine :10) sits between `#ifdef MUSY_BUILD_DEBUG`
//     (:9) and the nested `#if MUSY_VERSION < MUSY_VERSION_CHECK(1, 5, 1)`
//     (:13), so it belongs to every MUSY_BUILD_DEBUG build, the game's v1.5.3
//     revision included;
//   * the six functions (:14-105) are the ones inside the nested version guard,
//     whose comment reads "1.5 Patch 1 (note 31): the Gekko performance counters
//     are no longer used internally" - they exist only in MUSY_BUILD_DEBUG
//     builds older than v1.5.1, and the archive this image was linked from is
//     the v1.5.3 Dolphin one.
// The object this member compiles to has no .text, .data or .bss and so no
// image content: its only allocated section is a 0x20-byte .sdata2 holding the
// four weak constants `_half$localstatic3$sqrtf__Ff`,
// `_three$localstatic4$sqrtf__Ff`, `_half$localstatic3$sqrt__Ff` and
// `_three$localstatic4$sqrt__Ff` that <math.h>'s `extern inline sqrtf` (line 30)
// and `sqrt` (line 53) declare (src/ext/dolsdk2001/include/libc/math.h, reached
// through musyx.h:7).  No declaration here uses them, nothing references them,
// mwld drops them and the image is unaffected - the disposition hw_pc_vsample.c
// records for the same pair.
// Binary evidence that no profiling body was linked into this image:
//   * no sndProf* name exists in symbols.txt, and the only other user of
//     profile.h - hardware.c's `static SND_PROFILE_INFO prof` with its
//     PROF_START/PROF_PAUSE/PROF_STOP call sites (hardware.c:55-60) - sits behind
//     the same `MUSY_TARGET_DOLPHIN && MUSY_VERSION < 1.5.1 &&
//     defined(MUSY_BUILD_DEBUG)` guard, so this v1.5.3 release build defines no
//     profiling state and calls none of the six entry points.
//   * a decode of every mtspr/mfspr in orig/GC6E01/sys/main.dol puts every
//     PMC/MMCR-family access (SPRs 952-959) outside the MusyX runtime:
//     PPCArch.c's members write MMCR0 0x80098050 (PPCMtmmcr0), MMCR1 0x80098058
//     (PPCMtmmcr1), PMC1 0x80098060, PMC2 0x80098068, PMC3 0x80098070 and PMC4
//     0x80098078 (PPCMtpmc1..4) - all mtspr, no counter read - and MetroTRK
//     spills and restores them: TRKSaveExtended1Block (0x800C2A10) reads SDA
//     0x800C2B4C, PMC1 0x800C2B64, PMC2 0x800C2B68, PMC3 0x800C2B6C, PMC4
//     0x800C2B70, SIA 0x800C2B74, MMCR0 0x800C2B78 and MMCR1 0x800C2B7C, while
//     TRKRestoreExtended1Block (0x800C2BC8) writes PMC1-4/SIA/MMCR0/MMCR1 at
//     0x800C2C58-0x800C2C70.  Outside TRKSaveExtended1Block the image never
//     reads PMC1-4 at all, which is what latching them would require
//     (`info->_loadStores = PPCMfpmc2()` and the like in the reference bodies).
//   * inside the MusyX runtime .text run 0x8014635C-0x801653BC no PMC/MMCR SPR
//     is touched: the only SPRs any mtspr/mfspr in that run names are LR and
//     CTR.  splits.txt does not yet account for the whole run (0x80159C48-
//     0x8015D7D0, 0x8015FE88-0x8015FFDC and 0x80163EE0-0x801643D8 are still
//     open), so text coverage is not offered as the evidence here; the SPR scan
//     above spans those ranges too.
//   * the MMCR programming constants of the reference bodies, 0x78400000 (MMCR1)
//     and 0xC08B (MMCR0), occur as a literal word nowhere in the .text image.
// The member takes the zero-length .text slot at 0x801643D8 that the other
// non-emitting runtime.a members share (see its splits.txt entry).
// One marker per definition the reference text has:

// UNUSED: sndProfUserCallback; the optional user callback word, defined as NULL
// in any MUSY_BUILD_DEBUG build - it precedes the reference's v1.5.1 version
// guard.

// UNUSED: sndProfSetCallback; stores the user callback, MUSY_BUILD_DEBUG builds
// older than v1.5.1 only.

// UNUSED: sndProfUpdateMisc; fills numMusicVoices/numSFXVoices from
// voiceMusicRunning/voiceFxRunning, MUSY_BUILD_DEBUG builds older than v1.5.1
// only.

// UNUSED: sndProfResetPMC; clears PMC1-4 and the SND_PROFILE_DATA accumulators,
// MUSY_BUILD_DEBUG builds older than v1.5.1 only.

// UNUSED: sndProfStartPMC; programs MMCR1/MMCR0 (0x78400000/0xC08B) and latches
// the four counters, MUSY_BUILD_DEBUG builds older than v1.5.1 only.

// UNUSED: sndProfPausePMC; accumulates the counters and disables the monitor,
// MUSY_BUILD_DEBUG builds older than v1.5.1 only.

// UNUSED: sndProfStopPMC; accumulates, averages and peaks the counter deltas,
// MUSY_BUILD_DEBUG builds older than v1.5.1 only.

#endif

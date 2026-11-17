/* MusyX member src/musyx/runtime/hw_pc_vsample.c of the reference tree, the PC
 * implementation of the two virtual-sample buffer entry points
 * sndVirtualSampleAllocateBuffers/sndVirtualSampleFreeBuffers (CMakeLists.txt:53
 * lists it between hw_pc_rate.c (52) and hw_pc_song.c (54), among the other
 * PC-renderer members).  The game's Dolphin runtime.a extracts nothing from
 * it, so it contributes neither code nor data to main.dol:
 *
 *   * The pair it defines is the PC arm of a definition the shared reference
 *     source splits by target: pristine synth_vsamples.c:316-370 guards its own
 *     copies with `#if MUSY_TARGET != MUSY_TARGET_PC`, so exactly one of the two
 *     members can define the pair for a given target, and the archive this
 *     image was linked from is the Dolphin one (synth_vsamples.c's
 *     vsInit/vsSampleStartNotify/vsSampleEndNotify/vsUpdateBuffer/vsSampleUpdates
 *     are the linked virtual-sample code, 0x80159494-0x80159C48).
 *   * No sndVirtualSampleAllocateBuffers or sndVirtualSampleFreeBuffers address
 *     exists: symbols.txt names neither, and nothing in the image reaches either
 *     arm, so even the Dolphin arm's bodies are dead-stripped out of
 *     synth_vsamples.c's contiguous 0x7B4-byte run.
 *   * The calls these bodies would make are not made from anywhere that could
 *     be them.  A whole-image `bl` scan (owner from symbols.txt) finds
 *     aramAllocateStreamBuffer 0x80163CA8 called only from hwInitStream
 *     (0x80162F54), sndStreamAllocLength 0x8014F298 only from sndStreamAllocEx
 *     (0x8014F0B4) and aramFreeStreamBuffer 0x80163DE8 only from hwExitStream
 *     (0x80162F74) -- sndVirtualSampleAllocateBuffers must call the first two
 *     and rolls the third back on a short allocation count -- while hwOff
 *     0x80162D18 is called only from synthDeactivateStudio (0x8014DD20) and
 *     voiceKill 0x8015863C only from mcmdSetKeyGroup, synthHWMessageHandler,
 *     voiceKillSound and vsSampleUpdates, where this member's free path pairs
 *     voiceKill with hwOff inside its buffer loop.
 *   * The member keeps no storage of its own: its state is the shared `vs`
 *     object, whose .bss block at 0x80446F10 is the one synth_vsamples.c's split
 *     claims, and no `salPC`/`sndVirtualSample` string exists in the DOL for its
 *     host paths.
 *   * Its reference text is a host-toolchain member, not a CodeWarrior one: the
 *     archive's C89 -lang=c settings reject it outright.  Under that language
 *     mode musyx.h:44-50 takes its `#if __STDC_VERSION__ <= 199901L` arm (45-46)
 *     and only typedefs `bool` to unsigned long, the <stdbool.h> include (48)
 *     being the C11-and-later arm, so `false` at lines 9/16/20 and UINT32_MAX at
 *     line 15 are undefined identifiers while the mid-block declarations at
 *     lines 22-23 and the for-init declaration at line 50 are C89 syntax
 *     errors --
 *     compiling the pristine file with this object's exact command line reports
 *     exactly those.  MUSY_TARGET_PC together with C11 is what makes the
 *     reference CMake build accept the body.
 *
 * The translation unit keeps the reference includes so the member stays
 * accounted for and defines nothing, so it compiles to no .text and takes the
 * zero-length slot at 0x801643D8 shared by the dropped PC members
 * (hw_pc_null.c, hw_pc_output.c, hw_pc_rate.c, hw_pc_song.c, hw_pc_samples.c,
 * hw_pc_sdl.c).  Its only emitted section is the 0x20-byte .sdata2 holding the
 * weak `_half$localstatic3$sqrtf$Ff`/`_three$localstatic4$sqrtf$Ff` (and `sqrt`)
 * doubles that <math.h>'s extern inline sqrtf/sqrt contribute
 * (src/ext/dolsdk2001/include/libc/math.h:30-68, pulled in by musyx.h:7); no
 * declaration here uses them, nothing references them, mwld drops them, and the
 * image is unaffected. */
#include "musyx/hardware.h"
#include "musyx/vsamples.h"
#include "musyx/aramdma.h"
#include "musyx/voice.h"
#include <string.h>

// UNUSED: sndVirtualSampleAllocateBuffers; the PC body of the v1.5.3+ API
// (upstream guard `#if MUSY_VERSION >= MUSY_VERSION_CHECK(1, 5, 3)`).  It
// validates instances/samples/flags, takes hwDisableIrq, asks
// sndStreamAllocLength for the stream length, hands out aramAllocateStreamBuffer
// ids for `instances` VS_BUFFER slots and aramFreeStreamBuffer()s them again on
// a short count.  Neither the name nor those call sites appear in the image (see
// the file comment); the Dolphin arm in synth_vsamples.c owns the name for this
// target and is itself dropped at link because nothing calls it.

// UNUSED: sndVirtualSampleFreeBuffers; the PC body of the same API.  It walks
// vs.numBuffers, kills and switches off the voices of live buffers (voiceKill +
// hwOff) and aramFreeStreamBuffer()s every slot under hwDisableIrq.
// voiceKill/hwOff/aramFreeStreamBuffer are linked but called only from the sites
// listed above, never from a body of this shape.

/* MusyX member src/musyx/runtime/hw_pc_song.c of the reference tree, the PC
 * host song cache that CMakeLists.txt:54 lists between hw_pc_vsample.c and
 * hw_pc_samples.c.  Upstream it decodes an arrangement blob through the PC
 * decoder, reference-counts the decoded buffer in the file-static `songs` list
 * and frees it when the last referent is released.  The game's v1.5.3 Dolphin
 * runtime.a extracts nothing from it, so it contributes neither code nor data to
 * main.dol: with MUSY_TARGET == MUSY_TARGET_DOLPHIN the archive keeps no
 * undefined reference to any of its three exports (every call site is an arm of
 * `#if MUSY_TARGET == MUSY_TARGET_PC`, listed below), and none of the three
 * names appears in symbols.txt.  The translation unit keeps the reference
 * includes so the member stays accounted for, and takes the zero-length .text
 * slot at 0x801643D8 shared by the dropped PC members (hw_pc_null.c,
 * hw_pc_output.c, hw_pc_rate.c, hw_pc_samples.c, hw_pc_sdl.c). */
#include "hw_pc_assets.h"
#include "musyx/sal.h"

// UNUSED: PCSong (file-static: next/data/references) and the file-static list
// head `static PCSong *songs`.  The member is never extracted, so the cache head
// has no address in symbols.txt; the image's calls to hw_memory.c's allocator
// pair come only from the Dolphin-side functions listed at the end of this
// file, and none of them walks a song list.

// UNUSED: salPCAcquireSong; it looks the input up in `songs` and otherwise
// decodes it through salPCDecodeArrangement (hw_pc_assets.c:594) and links a
// node from salMalloc, this revision's salAlloc (0x801643D8).  Both call sites
// are PC-only arms with no `#else`/`#elif`: seq.c:409 (arm 408-412) and
// seq.c:937 (arm 932-945).

// UNUSED: salPCReleaseSong; it drops one reference from the matching `songs`
// entry.  All eight of its call sites are in seq.c and all are PC-only arms
// with no `#else`/`#elif`: :728 and :729 (arm 727-733), :746 (arm 745-749), :896
// (arm 895-899), :943 (arm 932-945), :1336 (arm 1335-1338) and :1583, :1584
// (arm 1582-1588).

// UNUSED: salPCCollectSongs; it unlinks and salFree()s every entry whose
// reference count reached zero.  Its five call sites are PC-only arms as well:
// s_data.c:561 (arm 338-681, `#else` at :567), s_data.c:940 (arm 936-943,
// `#else` at :944), snd_init.c:207 (arm 196-208), seq_api.c:49 (arm 48-50) and
// seq_api.c:139 (arm 138-140).

// The PC asset/decoder pair that salPCAcquireSong alone needs --
// salPCDecodeArrangement's hw_pc_assets.c/hw_pc_decode.c -- is absent from the
// image too, and a whole-image `bl` scan of main.dol (.text, owners from
// symbols.txt) finds the allocator pair this member would use called only from
// the Dolphin side: salAlloc (0x801643D8) from synthInit, salInitDspCtrl,
// aramInit, salInitAi and ReverbHICreate, salFree (0x80164400) from synthExit,
// salExitDspCtrl, aramInit, salExitAi and ReverbHIModify.  No linked body
// allocates or frees a decoded song buffer.

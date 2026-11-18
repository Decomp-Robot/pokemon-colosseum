#include "musyx/platform.h"

#if MUSY_TARGET == MUSY_TARGET_DOLPHIN

#include "dolphin/gx.h"
// Dolphin's OS prototypes use mutable strings; hide their declarations from
// MusyX's const-qualified declarations while keeping the actual imports intact.
#define OSReport __dolphin_OSReport
#define OSPanic __dolphin_OSPanic
#include "dolphin/os.h"
#undef OSPanic
#undef OSReport
#include "musyx/assert.h"
#include "musyx/txwin.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// txwin.a member src/musyx/txwin/txwin.c, the MusyX debug text-window front end
// (winOpenWindow/winLogPrintf/winRefresh and the sWIN list they keep).  The
// reference text is Dolphin-only - the whole body sits inside `#if MUSY_TARGET
// == MUSY_TARGET_DOLPHIN` (pristine :3) and musyx/txwin.h guards its
// declarations the same way - but for this target the member was compiled into
// txwin.a and never extracted from the archive: no game code references a win*
// entry point, so the linker dropped the whole object and the image carries
// none of it.  This file therefore keeps the includes and one marker per
// definition the reference has, the disposition debugger.a, profile.c and the
// other unextracted MusyX/Dolphin members record, and the object it compiles to
// has no .text, .data or .bss and so no image content.
// Binary evidence, checked against orig/GC6E01/sys/main.dol:
//   * winInit's body is its own fingerprint: `GXSetCopyClear((GXColor){0x0A,
//     0x10, 0x19, 0xFF}, 0xFFFFFF)`.  Built with this project's GC/2.6 release
//     flags the colour word 0x0A1019FF is the first entry of the object's
//     0x28-byte .sdata2 (beside the 8.0/640.0/0.75 double constants of
//     winSetFontSize), and that word occurs nowhere in the image - not in
//     .text, .rodata, .data, .sdata or .sdata2.  The test is independent of the
//     assert level.
//   * no win* name exists in symbols.txt, and none of the member's strings is in
//     the image ("TXWIN: Illegal X coords for window\n", "TXWIN: Illegal y coords
//     for window\n", "TXWIN: FAILED TO ALLOCATE WINDOW!\n", "TXWIN: Unable to
//     allocate log window buffer.\n", "TXWIN: Failed to allocate buffer
//     element\n", ">> winRefresh(): window list is empty!\n", "__add_node():
//     you're adding a NULL node!\n", "__delete_node(): you're deleting a NULL
//     node!\n", "OHMYGAWD\n").  MUSY_ASSERT_MSG only expands under
//     MUSY_BUILD_DEBUG (musyx/assert.h) and this archive is built release-style
//     (-DNDEBUG=1, no _DEBUG on the command line), so that absence corroborates
//     the point above rather than proving it.
//   * the member's three demo-library imports - DEMOGetRenderModeObj and
//     DEMOPrintf (declared at the top of the reference) and DEMOInitCaption
//     (called by winRefresh) - have no definition in the image either:
//     symbols.txt holds no DEMO* name, `strings` finds no "DEMO" text, and all
//     six demo.a members this project carries (DEMOInit/DEMOPuts/DEMOFont/
//     DEMOWin/DEMOPad/DEMOStats) sit on the shared zero-length .text slot
//     0x800CF708 in splits.txt, i.e. they were never extracted.  Extracting the
//     txwin.a member would have forced those three demo members into the link.
//   * mwld states the same limit: force-linking the reference bodies (the
//     experiment of compiling them against a .text split at 0x801653BC) fails
//     with "multiply-defined: '__rmp' in txwin.o / Previously defined in
//     DEMOWin.o" - __rmp is a global here (pristine :24) and, under the same
//     name, in dolsdk2001's demo/DEMOWin.c:12.  An archive link holds at most
//     one definition, so the txwin.a member and the demo library were never in
//     one link, and the demos are not in the image.
//   * no unattributed .text span is left for the member: MusyX text ends at
//     0x801653BC, where game code resumes (zz_801653BC, zz_801653C4, ...), and
//     the spans splits.txt cannot attribute yet are MusyX audio/DSP service code
//     (0x80163EE0-0x801643D8: AIInitDMA/OSEnableInterrupts stream callbacks) and
//     snd_* table/lookup code (0x80159C48-0x8015D7D0, 0x8015FE88-0x8015FFDC),
//     with no GX, window or demo-library call site in either.
// The member takes the zero-length .text slot 0x801653BC..0x801653BC that
// follows debugger.a's in link order (see its splits.txt entry).
// Compiler: GC/2.6, the MusyX archive default shared with the runtime.a and
// debugger.a members; the archive's include tree (musyx headers and the
// dolsdk2001 include tree plus its libc) and MUSY_TARGET_DOLPHIN with
// MUSY_VERSION 1.5.3 follow the sibling MusyX members.
//
// The reference keeps five declarations at file scope above its definitions:
// the two temporary demo-library prototypes (DEMOGetRenderModeObj :15 and
// DEMOPrintf :16, under its /* TODO: TEMPORARY HACKS */ note at :14) and three
// forward declarations for the __win_* helpers (:18-20).  With no definitions
// left to serve, they go too.  The OSReport/OSPanic rename above is the only
// adaptation: dolsdk2001 declares those two with mutable char* while
// musyx/assert.h declares them const-qualified, and the reference includes both
// - the same rename stream.c carries.
// One marker per definition the reference text has:

// UNUSED: __X_Res; horizontal resolution the caption/font metrics are derived
// from, set by winSetFontSize.

// UNUSED: __Y_Res; vertical resolution, 0.75 x __X_Res.

// UNUSED: __rmp; the GXRenderModeObj latched from DEMOGetRenderModeObj by
// winInit.  The demo library defines the same global, see the note above.

// UNUSED: __CurrNode; current node of the window list.

// UNUSED: __LastNode; tail of the window list, appended to by __win_add_node.

// UNUSED: __FirstNode; head of the window list, walked by winRefresh.

// UNUSED: winInit; clears the three list pointers, sets the font size to 8,
// latches __rmp and sets the GX copy-clear colour to 0x0A1019FF.

// UNUSED: winOpenWindow; OSAlloc's an sWIN, appends it to the list and fills in
// the geometry, character metrics, caption, refresh hook and flags.

// UNUSED: winOpenLogWindow; opens a window with the log refresh hook, sizes the
// ring buffer from char_height + num_lines and allocates one line buffer per
// row.

// UNUSED: winPrintfXY; vsprintf's into a 128-byte buffer, clips it to
// char_width and DEMOPrintf's it at the given character cell.

// UNUSED: winLogPrintf; vsprintf's into a 128-byte buffer and strcpy's it into
// the current output line of the ring buffer.

// UNUSED: winClearLogWindow; zeroes every line buffer and resets the output and
// view line cursors.

// UNUSED: winSetFontSize; maps a font size in 8..40 to __X_Res = 640 * 8/size
// and __Y_Res = 0.75 * __X_Res, else to 640x480.

// UNUSED: winRefresh; sets up the caption from __X_Res/__Y_Res and calls the
// refresh hook of every window in the list.

// UNUSED: __win_add_node; appends a window to the doubly-linked list, or makes
// it the first node.

// UNUSED: __win_delete_node; unlinks a window from the head, tail or middle of
// the list and OSFree's it.

// UNUSED: __win_log_refresh; redraws the visible log lines bottom-up, walking
// the ring buffer backwards from curr_output_line with DEMOPrintf.

#endif // MUSY_TARGET == MUSY_TARGET_DOLPHIN

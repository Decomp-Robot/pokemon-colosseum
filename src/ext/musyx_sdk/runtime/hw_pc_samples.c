/* MusyX member src/musyx/runtime/hw_pc_samples.c (reference: MusyX 2.0.0 PC
 * build).  The game's v1.5.3 Dolphin runtime.a keeps this sample registry in
 * synthdata.c instead, inside its `#if MUSY_TARGET != MUSY_TARGET_PC` arm, so
 * the member contributes no bytes to the image: every definition below is
 * either that same code under the name synthdata.c links, or a PC-only helper
 * whose only caller is the PC renderer/target arm.  The translation unit keeps
 * the reference includes so the member stays accounted for. */
#include "hw_pc_assets.h"
#include "musyx/hardware.h"
#include <string.h>

// UNUSED: static SDIR_TAB directories[128], directoryCount, uploading; PC
// sample-registry state. The image's directory table is synthdata.c's
// dataSmpSDirs/dataSmpSDirNum, and the member's 0x600-byte table is absent.

// UNUSED: salPCResetSampleData; called only from synthdata.c's dataInit under
// `#if MUSY_TARGET == MUSY_TARGET_PC`, an arm the Dolphin build compiles out.

// UNUSED: salPCFinishSampleUpload; called only from this member's
// dataRemoveSDir and from s_data.c, both PC-build paths.

// UNUSED: findSample (file-static); sorted-array lookup used by the PC
// registry's activeSample/dataAddSampleReference. The image's lookups are
// synthdata.c's smpcmp comparator plus sndBSearch (0x80162118).

// UNUSED: activeSample (file-static); walks the PC `directories` array, which
// the image does not contain.

// UNUSED: dataInsertSDir; the image's dataInsertSDir (0x80151770, 0x188 bytes)
// is synthdata.c's arm -- hwDisableIrq/hwEnableIrq around the 128-entry
// dataSmpSDirs table and the per-entry duplicate-id scan -- not this member's
// `uploading`-pointer version, which has no room in the linked archive range.

// UNUSED: dataRemoveSDir; the image has no dataRemoveSDir address at all: in
// the v1.5.3 synthdata.c arm its only caller is compiled out.  That call sits
// in dataRemoveSampleReference's `#if MUSY_VERSION < MUSY_VERSION_CHECK(1, 5, 1)`
// path (pristine synthdata.c:469), which the 1.5.3 build does not take - the
// `#else` branch only decrements ref_cnt - so the unreferenced body is dropped
// by the link, and this member's separate implementation is never extracted.

// UNUSED: dataAddSampleReference; the image's 0x801518F8 (0xD8) is
// synthdata.c's dataSmpSDirs/ref_cnt version; this member's variant instead
// resolves the entry through `uploading` and stores it in `directories`.

// UNUSED: dataRemoveSampleReference; the image's 0x801519D0 (0x98) is
// synthdata.c's version over dataSmpSDirs.

// UNUSED: dataGetSample; the image's 0x801521B8 (0x128) is synthdata.c's
// version, which searches with the smpcmp comparator at 0x801521A8 and reads
// SAMPLE_INFO from dataSmpSDirs; this member's binary search over `directories`
// is not in the image.

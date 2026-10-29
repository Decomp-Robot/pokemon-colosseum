#include <dolphin/dsp.h>

#include "__dsp.h"

/* This member owns the DSP task list heads; dsp_task.c only references them
 * (as __dsp.h declares), so the four words are defined exactly once in the
 * link. The declaration order below is load-bearing: the retail image stores
 * them in .sbss as __DSP_tmp_task 0x8047A960, __DSP_last_task 0x8047A964,
 * __DSP_first_task 0x8047A968, __DSP_curr_task 0x8047A96C -- the reverse of
 * this order, which is how GC/1.2.5n lays out external small-data objects.
 * The addresses are visible in the code that touches them (DSPInit stores
 * NULL to __DSP_tmp_task/__DSP_curr_task/__DSP_last_task/__DSP_first_task at
 * 0x800AE878..0x800AE884, DSPAddTask reads __DSP_first_task at 0x800AE97C,
 * and __DSPHandler reads __DSP_curr_task at 0x800AEA4C). The block starts right
 * after dsp.o's 4-byte .sbss word at 0x8047A950 and the three still-unattributed
 * words at 0x8047A954..0x8047A960 (two of them are read/written by __DSPHandler
 * at 0x800AEAFC/0x800AEB28), and it ends before the separate object at
 * 0x8047A970. */
DSPTaskInfo *__DSP_curr_task;
DSPTaskInfo *__DSP_first_task;
DSPTaskInfo *__DSP_last_task;
DSPTaskInfo *__DSP_tmp_task;

void __DSP_debug_printf(const char *fmt, ...) {}

// UNUSED: the game never calls this; dead-stripped at link.
DSPTaskInfo *__DSPGetCurrentTask(void)
{
    return __DSP_curr_task;
}

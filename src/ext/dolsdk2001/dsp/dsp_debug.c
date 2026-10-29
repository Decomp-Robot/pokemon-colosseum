#include <dolphin/dsp.h>

#include "__dsp.h"

/* dsp_task.c owns the DSP task list heads (__dsp.h declares them for the rest
 * of the archive), so this member only references them. */
void __DSP_debug_printf(const char *fmt, ...) {}

// UNUSED: the game never calls this; dead-stripped at link.
DSPTaskInfo *__DSPGetCurrentTask(void)
{
    return __DSP_curr_task;
}

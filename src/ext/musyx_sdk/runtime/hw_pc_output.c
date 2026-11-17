#include "hw_pc_internal.h"
#include "musyx/hardware.h"
#include "musyx/synth.h"

#include <string.h>

// UNUSED: sndPCSetOutputCallback; PC renderer API, not in the Dolphin archive.

// UNUSED: sndPCSetSynchronization; PC renderer API, not in the Dolphin archive.

// UNUSED: salPCExternalEnter; only the PC interrupt stubs call it, and the Dolphin
// archive takes those from hw_dolphin.c.

// UNUSED: salPCExternalLeave; only the PC interrupt stubs call it, and the Dolphin
// archive takes those from hw_dolphin.c.

// UNUSED: sndPCConfigure; PC renderer API, not in the Dolphin archive.

// UNUSED: salPCMixRate; the Dolphin build compiles out its only call site
// (synth_vsamples.c).

// UNUSED: salPCChannels; the Dolphin build compiles out its only call site
// (hw_volconv.c).

// UNUSED: salPCResetOutput; reached only from the PC host backend, which is not in
// the Dolphin archive.

// UNUSED: salAiGetDest; the Dolphin archive takes the AI buffer destination from
// hw_dolphin.c.

// UNUSED: sndPCRender; PC renderer API, not in the Dolphin archive.

// UNUSED: sndPCGetRenderInfo; PC renderer API, not in the Dolphin archive.

// UNUSED: salStartAi; the Dolphin archive takes the AI start hook from hw_dolphin.c.

// UNUSED: salExitAi; the Dolphin archive takes the AI exit hook from hw_dolphin.c.

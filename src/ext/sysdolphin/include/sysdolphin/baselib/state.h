#ifndef SYSDOLPHIN_BASELIB_STATE_H
#define SYSDOLPHIN_BASELIB_STATE_H

#include <dolphin/types.h>
#include <dolphin/gx/GXStruct.h>

typedef enum _HSD_RenderPass {
    HSD_RENDER_PASS_SCALED,
    HSD_RENDER_PASS_DIRECT,
    HSD_RENDER_PASS_PROJECTION_SCALED,
    HSD_RENDER_PASS_NORMAL
} HSD_RenderPass;

extern GXRenderModeObj RenderMode;

void HSD_RenderStateClear(void);
void HSD_RenderStateUpdate(void);
void HSD_RenderStateFlush(void);
HSD_RenderPass HSD_RenderPassGet(void);

#endif

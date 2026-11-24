#ifndef SYSDOLPHIN_BASELIB_FOBJ_H
#define SYSDOLPHIN_BASELIB_FOBJ_H

#include "sysdolphin/baselib/aobj.h"

extern HSD_FObj* HSD_FObjLoadDesc(HSD_FObjDesc* desc);
extern void HSD_FObjInterpretAnim(HSD_FObj* fobj, void* obj, HSD_AObjUpdateCallback cb, f32 step);
extern void HSD_FObjStop(HSD_FObj* fobj, void* obj, HSD_AObjUpdateCallback cb, f32 step);
extern void HSD_FObjSetFrame(HSD_FObj* fobj, f32 frame);
extern void HSD_FObjRemove(HSD_FObj* fobj);

#endif

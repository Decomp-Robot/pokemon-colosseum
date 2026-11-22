#ifndef SYSDOLPHIN_BASELIB_AOBJ_H
#define SYSDOLPHIN_BASELIB_AOBJ_H

#include <dolphin/types.h>

#include <sysdolphin/baselib/object.h>

typedef struct _HSD_AObjDesc HSD_AObjDesc;
typedef struct _HSD_FObj HSD_FObj;

/* Pool-allocated animation record interpreted once per frame. */
typedef struct _HSD_AObj {
    u32 flags;
    f32 curr_frame;
    f32 start_frame;
    f32 end_frame;
    f32 framerate;
    HSD_FObj* fobj;
    HSD_Obj* obj;
} HSD_AObj;

void HSD_AObjRelease(HSD_AObj* aobj);
HSD_AObj* HSD_AObjLoad(HSD_AObjDesc* desc);
void HSD_AObjInterpret(HSD_AObj* aobj, void* owner, void (*applier)(void*, s32, f32*));
void HSD_AObjSetFrame(HSD_AObj* aobj, f32 frame);

#endif

#ifndef SYSDOLPHIN_BASELIB_ROBJ_H
#define SYSDOLPHIN_BASELIB_ROBJ_H

#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>

typedef struct _HSD_RObj HSD_RObj;
typedef struct _HSD_RObjDesc HSD_RObjDesc;
typedef struct _HSD_RObjAnimDesc HSD_RObjAnimDesc;

struct _HSD_RObj {
    struct _HSD_RObj* next;
    u8 unk_04[20];
    HSD_AObj* aobj;
};

void HSD_RObjRelease(HSD_RObj* robj);
HSD_RObj* HSD_RObjLoad(HSD_RObjDesc* desc);
void HSD_RObjResolveRefs(HSD_RObj* robj, HSD_RObjDesc* desc);
void HSD_RObjAddAnim(HSD_RObj* robj, HSD_RObjAnimDesc* desc);
void HSD_RObjSetFrame(HSD_RObj* robj, f32 frame);
void HSD_RObjRemoveAnim(HSD_RObj* robj);
void HSD_RObjInterpretAnim(HSD_RObj* robj);

#endif

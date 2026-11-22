#ifndef SYSDOLPHIN_BASELIB_WOBJ_H
#define SYSDOLPHIN_BASELIB_WOBJ_H

#include <dolphin/mtx.h>
#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/class.h>
#include <sysdolphin/baselib/object.h>
#include <sysdolphin/baselib/robj.h>

typedef struct _HSD_WObjDesc {
    char* class_name;
    Vec position;
    HSD_RObjDesc* robjdesc;
} HSD_WObjDesc;

typedef struct _HSD_WObjAnimDesc {
    HSD_AObjDesc* aobjdesc;
    HSD_RObjAnimDesc* robjanimdesc;
} HSD_WObjAnimDesc;

typedef struct _HSD_WObj {
    HSD_Obj obj;
    u32 flags;
    Vec pos;
    HSD_AObj* aobj;
    HSD_RObj* robj;
} HSD_WObj;

/* Animation-track codes passed to the update method by the AObj interpreter. */
typedef enum _HSD_WObjTrack {
    HSD_WOBJ_TRACK_PATH = 4,
    HSD_WOBJ_TRACK_X = 5,
    HSD_WOBJ_TRACK_Y = 6,
    HSD_WOBJ_TRACK_Z = 7
} HSD_WObjTrack;

typedef struct _HSD_WObjInfo {
    HSD_ClassInfo parent;
    s32 (*load)(HSD_WObj* wobj, HSD_WObjDesc* desc);
    void (*update)(void* obj, s32 cmd, f32* value);
} HSD_WObjInfo;

extern HSD_WObjInfo hsd_WObjInfo;

void HSD_WObjInit(void);
void HSD_WObjAmnesia(HSD_ClassInfo* info);
void HSD_WObjRelease(void* obj);
HSD_WObj* HSD_WObjAlloc(void);
void HSD_WObjGetPosition(HSD_WObj* wobj, Vec* pos);
void HSD_WObjSetPosition(HSD_WObj* wobj, Vec* pos);
HSD_WObj* HSD_WObjLoad(HSD_WObjDesc* desc);
void HSD_WObjInitDesc(HSD_WObj* wobj, HSD_WObjDesc* desc);
s32 HSD_WObjLoadDesc(HSD_WObj* wobj, HSD_WObjDesc* desc);
void HSD_WObjInterpretAnim(HSD_WObj* wobj);
void HSD_WObjUpdate(void* obj, s32 cmd, f32* value);
void HSD_WObjAddAnim(HSD_WObj* wobj, HSD_WObjAnimDesc* desc);
void HSD_WObjSetFrame(HSD_WObj* wobj, f32 frame);
void HSD_WObjRemoveAnim(HSD_WObj* wobj);

#endif

#ifndef SYSDOLPHIN_BASELIB_AOBJ_H
#define SYSDOLPHIN_BASELIB_AOBJ_H

#include <dolphin/types.h>
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/objalloc.h"

typedef struct _HSD_JObj HSD_JObj;
typedef struct _HSD_FObj HSD_FObj;
typedef struct _HSD_FObjDesc HSD_FObjDesc;
typedef struct _HSD_JObjDesc HSD_JObjDesc;

typedef struct _HSD_AObj {
    u32 flags;
    f32 curr_frame;
    f32 start_frame;
    f32 end_frame;
    f32 framerate;
    HSD_FObj* fobj;
    HSD_JObj* obj;
} HSD_AObj;

typedef struct _HSD_AObjDesc {
    u32 flags;
    f32 frame;
    HSD_FObjDesc* fobj;
    HSD_JObjDesc* objdesc;
} HSD_AObjDesc;

typedef enum _AObjNotifyFormat {
    AOBJ_NOTIFY_NONE,
    AOBJ_NOTIFY_F32,
    AOBJ_NOTIFY_S32,
    AOBJ_NOTIFY_U32,
    AOBJ_NOTIFY_PTR,
    AOBJ_NOTIFY_PTR_F32,
    AOBJ_NOTIFY_PTR_S32,
    AOBJ_NOTIFY_PTR_U32,
    AOBJ_NOTIFY_PTR_KIND,
    AOBJ_NOTIFY_PTR_KIND_F32,
    AOBJ_NOTIFY_PTR_KIND_S32,
    AOBJ_NOTIFY_PTR_KIND_U32
} AObjNotifyFormat;

typedef enum _AObjObjectKind {
    AOBJ_OBJECT_KIND_COBJ = 2,
    AOBJ_OBJECT_KIND_DOBJ,
    AOBJ_OBJECT_KIND_FOG = 5,
    AOBJ_OBJECT_KIND_JOBJ,
    AOBJ_OBJECT_KIND_LOBJ,
    AOBJ_OBJECT_KIND_MOBJ,
    AOBJ_OBJECT_KIND_POBJ,
    AOBJ_OBJECT_KIND_ROBJ,
    AOBJ_OBJECT_KIND_TOBJ,
    AOBJ_OBJECT_KIND_WOBJ
} AObjObjectKind;

typedef enum _AObjVisitFlag {
    AOBJ_VISIT_COBJ = 1 << (AOBJ_OBJECT_KIND_COBJ - 1),
    AOBJ_VISIT_DOBJ = 1 << (AOBJ_OBJECT_KIND_DOBJ - 1),
    AOBJ_VISIT_FOG = 1 << (AOBJ_OBJECT_KIND_FOG - 1),
    AOBJ_VISIT_JOBJ = 1 << (AOBJ_OBJECT_KIND_JOBJ - 1),
    AOBJ_VISIT_LOBJ = 1 << (AOBJ_OBJECT_KIND_LOBJ - 1),
    AOBJ_VISIT_MOBJ = 1 << (AOBJ_OBJECT_KIND_MOBJ - 1),
    AOBJ_VISIT_POBJ = 1 << (AOBJ_OBJECT_KIND_POBJ - 1),
    AOBJ_VISIT_ROBJ = 1 << (AOBJ_OBJECT_KIND_ROBJ - 1),
    AOBJ_VISIT_TOBJ = 1 << (AOBJ_OBJECT_KIND_TOBJ - 1),
    AOBJ_VISIT_WOBJ = 1 << (AOBJ_OBJECT_KIND_WOBJ - 1)
} AObjVisitFlag;

typedef void (*HSD_AObjCallback)(HSD_AObj* aobj, ...);
typedef void (*AObjCallbackNoValue)(HSD_AObj* aobj);
typedef void (*AObjCallbackFloat)(HSD_AObj* aobj, f32 value);
typedef void (*AObjCallbackS32)(HSD_AObj* aobj, s32 value);
typedef void (*AObjCallbackU32)(HSD_AObj* aobj, u32 value);
typedef void (*AObjCallbackOwner)(HSD_AObj* aobj, void* owner);
typedef void (*AObjCallbackOwnerFloat)(HSD_AObj* aobj, void* owner, f32 value);
typedef void (*AObjCallbackOwnerS32)(HSD_AObj* aobj, void* owner, s32 value);
typedef void (*AObjCallbackOwnerU32)(HSD_AObj* aobj, void* owner, u32 value);
typedef void (*AObjCallbackOwnerKind)(HSD_AObj* aobj, void* owner, AObjObjectKind kind);
typedef void (*AObjCallbackOwnerKindFloat)(HSD_AObj* aobj, void* owner, AObjObjectKind kind, f32 value);
typedef void (*AObjCallbackOwnerKindS32)(HSD_AObj* aobj, void* owner, AObjObjectKind kind, s32 value);
typedef void (*AObjCallbackOwnerKindU32)(HSD_AObj* aobj, void* owner, AObjObjectKind kind, u32 value);

typedef void (*HSD_AObjUpdateCallback)(void* obj, s32 track, f32* value);

typedef enum _AObjFlag {
    AOBJ_STOPPED = 0x40000000u,
    AOBJ_LOOP = 0x20000000u,
    AOBJ_NO_UPDATE = 0x10000000u,
    AOBJ_FIRST_FRAME = 0x08000000u,
    AOBJ_LOOPED = 0x04000000u,
    AOBJ_FRAME_FLAGS = AOBJ_LOOP | AOBJ_NO_UPDATE
} AObjFlag;

static inline f32 HSD_AObjGetEndFrame(HSD_AObj* aobj)
{
    HSD_ASSERT(171, aobj);
    return aobj->end_frame;
}

void HSD_AObjInit(void);
void HSD_AObjSetRate(HSD_AObj* aobj, f32 rate);
void HSD_AObjWalk(void* obj, AObjObjectKind obj_type, u32 mask,
                  HSD_AObjCallback callback, AObjNotifyFormat value_type, ...);
void HSD_AObjRelease(HSD_AObj* aobj);
HSD_AObj* HSD_AObjLoad(HSD_AObjDesc* desc);
void HSD_AObjInterpret(HSD_AObj* aobj, void* obj, HSD_AObjUpdateCallback cb);
void HSD_AObjSetFrame(HSD_AObj* aobj, f32 frame);
void HSD_AObjProcessAnimCb(void);
void HSD_AObjResetAnimCb(void);
void HSD_AObjClearFlags(HSD_AObj* aobj, u32 flags);
void HSD_AObjSetFlags(HSD_AObj* aobj, u32 flags);
HSD_ObjAllocData* HSD_AObjGetAllocData(void);
void HSD_AObjInitAllocData(void);

#endif

#ifndef SYSDOLPHIN_BASELIB_JOBJ_H
#define SYSDOLPHIN_BASELIB_JOBJ_H

#include <dolphin/mtx.h>
#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/debug.h>
#include <sysdolphin/baselib/dobj.h>
#include <sysdolphin/baselib/object.h>
#include <sysdolphin/baselib/robj.h>
#include <sysdolphin/baselib/spline.h>

typedef struct _HSD_JObjDesc HSD_JObjDesc;

typedef struct _HSD_JObj {
    HSD_Obj obj;
    struct _HSD_JObj* next;
    struct _HSD_JObj* parent;
    struct _HSD_JObj* child;
    u32 flags;
    union {
        HSD_Spline* spline;
        HSD_DObj* dobj;
    } u;
    Quaternion rotation;
    Vec scale;
    Vec translate;
    Mtx mtx;
    Vec* scale_factor;
    MtxPtr envelopemtx;
    HSD_AObj* aobj;
    HSD_RObj* robj;
    HSD_JObjDesc* desc;
} HSD_JObj;

#define JOBJ_USER_DEF_MTX 0x00800000
#define JOBJ_SPLINE 0x00000020
#define JOBJ_MTX_DIRTY 0x00000040
#define JOBJ_INSTANCE 0x00001000
#define JOBJ_PTCL 0x00004000

extern void HSD_JObjUpdateMtx(HSD_JObj* jobj);
extern void HSD_JObjUnref(HSD_JObj* obj);
extern HSD_JObj* HSD_JObjLoadDesc(HSD_JObjDesc* desc);

static inline BOOL HSD_JObjMtxIsDirty(HSD_JObj* jobj)
{
    BOOL update;

    HSD_ASSERT(605, jobj);
    update = FALSE;
    if (!(jobj->flags & JOBJ_USER_DEF_MTX) && (jobj->flags & JOBJ_MTX_DIRTY)) {
        update = TRUE;
    }
    return update;
}

static inline void HSD_JObjSetupMatrix(HSD_JObj* jobj)
{
    if (jobj != NULL && HSD_JObjMtxIsDirty(jobj)) {
        HSD_JObjUpdateMtx(jobj);
    }
}

#endif

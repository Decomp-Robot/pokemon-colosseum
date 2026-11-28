#include <dolphin/types.h>
#include <stdarg.h>
#include <string.h>
#include "sysdolphin/baselib/aobj.h"
#include "sysdolphin/baselib/cobj.h"
#include "sysdolphin/baselib/dobj.h"
#include "sysdolphin/baselib/fobj.h"
#include "sysdolphin/baselib/fog.h"
#include "sysdolphin/baselib/id.h"
#include "sysdolphin/baselib/jobj.h"
#include "sysdolphin/baselib/lobj.h"
#include "sysdolphin/baselib/mobj.h"
#include "sysdolphin/baselib/objalloc.h"
#include "sysdolphin/baselib/object.h"
#include "sysdolphin/baselib/pobj.h"
#include "sysdolphin/baselib/robj.h"
#include "sysdolphin/baselib/tobj.h"
#include "sysdolphin/baselib/wobj.h"

extern f64 fmod(f64 x, f64 y);

typedef struct _AObjCallbackNode {
    struct _AObjCallbackNode* next;
    void (*callback)(void);
} AObjCallbackNode;

typedef union AObjValue {
    f32 f;
    s32 i;
    u32 u;
} AObjValue;

#define AObjNotify(cb, fmt, aobj, owner, kind, v) \
    do { \
        HSD_AObj* aobj_ = (aobj); \
        switch (fmt) { \
        case AOBJ_NOTIFY_NONE: \
            ((AObjCallbackNoValue) (cb))(aobj_); \
            break; \
        case AOBJ_NOTIFY_F32: \
            ((AObjCallbackFloat) (cb))(aobj_, (v).f); \
            break; \
        case AOBJ_NOTIFY_S32: \
            ((AObjCallbackS32) (cb))(aobj_, (v).i); \
            break; \
        case AOBJ_NOTIFY_U32: \
            ((AObjCallbackU32) (cb))(aobj_, (v).u); \
            break; \
        case AOBJ_NOTIFY_PTR: \
            ((AObjCallbackOwner) (cb))(aobj_, (owner)); \
            break; \
        case AOBJ_NOTIFY_PTR_KIND: \
            ((AObjCallbackOwnerKind) (cb))(aobj_, (owner), (kind)); \
            break; \
        case AOBJ_NOTIFY_PTR_F32: \
            ((AObjCallbackOwnerFloat) (cb))(aobj_, (owner), (v).f); \
            break; \
        case AOBJ_NOTIFY_PTR_S32: \
            ((AObjCallbackOwnerS32) (cb))(aobj_, (owner), (v).i); \
            break; \
        case AOBJ_NOTIFY_PTR_U32: \
            ((AObjCallbackOwnerU32) (cb))(aobj_, (owner), (v).u); \
            break; \
        case AOBJ_NOTIFY_PTR_KIND_F32: \
            ((AObjCallbackOwnerKindFloat) (cb))(aobj_, (owner), (kind), (v).f); \
            break; \
        case AOBJ_NOTIFY_PTR_KIND_S32: \
            ((AObjCallbackOwnerKindS32) (cb))(aobj_, (owner), (kind), (v).i); \
            break; \
        case AOBJ_NOTIFY_PTR_KIND_U32: \
            ((AObjCallbackOwnerKindU32) (cb))(aobj_, (owner), (kind), (v).u); \
            break; \
        } \
    } while (0)

static void AObjWalkJObj(HSD_JObj* jobj, u32 mask, HSD_AObjCallback cb,
                         AObjNotifyFormat fmt, AObjValue* value);
static void AObjWalkDObj(HSD_DObj* dobj, u32 mask, HSD_AObjCallback cb,
                         AObjNotifyFormat fmt, AObjValue* value);
static void AObjWalkLObj(HSD_LObj* lobj, u32 mask, HSD_AObjCallback cb,
                         AObjNotifyFormat fmt, AObjValue* value);
static void AObjWalkCObj(HSD_CObj* cobj, u32 mask, HSD_AObjCallback cb,
                         AObjNotifyFormat fmt, AObjValue* value);

HSD_ObjAllocData hsdAObjAllocData;
static s32 aobjStoppedCount;
static s32 aobjActiveCount;
static AObjCallbackNode* aobjAnimCallbackList;

void HSD_AObjInit(void)
{
    aobjAnimCallbackList = NULL;
}

void HSD_AObjSetRate(HSD_AObj* aobj, f32 rate)
{
    if (aobj != NULL) {
        aobj->framerate = rate;
    }
}

void HSD_AObjWalk(void* obj, AObjObjectKind obj_type, u32 mask,
                  HSD_AObjCallback callback, AObjNotifyFormat value_type, ...)
{
    AObjValue value;
    va_list ap;

    if (obj == NULL) {
        return;
    }

    va_start(ap, value_type);
    switch (value_type) {
    case AOBJ_NOTIFY_NONE:
    case AOBJ_NOTIFY_PTR:
    case AOBJ_NOTIFY_PTR_KIND:
        break;
    case AOBJ_NOTIFY_F32:
    case AOBJ_NOTIFY_PTR_F32:
    case AOBJ_NOTIFY_PTR_KIND_F32:
        value.f = va_arg(ap, f64);
        break;
    case AOBJ_NOTIFY_S32:
    case AOBJ_NOTIFY_PTR_S32:
    case AOBJ_NOTIFY_PTR_KIND_S32:
        value.i = va_arg(ap, s32);
        break;
    case AOBJ_NOTIFY_U32:
    case AOBJ_NOTIFY_PTR_U32:
    case AOBJ_NOTIFY_PTR_KIND_U32:
        value.u = va_arg(ap, u32);
        break;
    default:
        HSD_ERR(826, "unexpected argument format.\n");
        break;
    }
    va_end(ap);

    switch (obj_type) {
    case AOBJ_OBJECT_KIND_JOBJ:
        AObjWalkJObj(obj, mask, callback, value_type, &value);
        break;
    case AOBJ_OBJECT_KIND_DOBJ:
        AObjWalkDObj(obj, mask, callback, value_type, &value);
        break;
    case AOBJ_OBJECT_KIND_MOBJ:
        if (obj != NULL) {
            HSD_TObj* tobj;
            if ((mask & AOBJ_VISIT_MOBJ) != 0 && ((HSD_MObj*) obj)->aobj != NULL) {
                AObjNotify(callback, value_type, ((HSD_MObj*) obj)->aobj, obj, AOBJ_OBJECT_KIND_MOBJ, value);
            }
            for (tobj = ((HSD_MObj*) obj)->tobj; tobj != NULL; tobj = tobj->next) {
                if ((mask & AOBJ_VISIT_TOBJ) != 0 && tobj->aobj != NULL) {
                    AObjNotify(callback, value_type, tobj->aobj, tobj, AOBJ_OBJECT_KIND_TOBJ, value);
                }
            }
        }
        break;
    case AOBJ_OBJECT_KIND_POBJ: {
        HSD_PObj* pobj;
        for (pobj = obj; pobj != NULL; pobj = pobj->next) {
            if ((mask & AOBJ_VISIT_POBJ) != 0 && pobj->aobj != NULL) {
                AObjNotify(callback, value_type, pobj->aobj, pobj, AOBJ_OBJECT_KIND_POBJ, value);
            }
        }
        break;
    }
    case AOBJ_OBJECT_KIND_TOBJ: {
        HSD_TObj* tobj;
        for (tobj = obj; tobj != NULL; tobj = tobj->next) {
            if ((mask & AOBJ_VISIT_TOBJ) != 0 && tobj->aobj != NULL) {
                AObjNotify(callback, value_type, tobj->aobj, tobj, AOBJ_OBJECT_KIND_TOBJ, value);
            }
        }
        break;
    }
    case AOBJ_OBJECT_KIND_LOBJ:
        AObjWalkLObj(obj, mask, callback, value_type, &value);
        break;
    case AOBJ_OBJECT_KIND_COBJ:
        AObjWalkCObj(obj, mask, callback, value_type, &value);
        break;
    case AOBJ_OBJECT_KIND_ROBJ: {
        HSD_RObj* robj;
        for (robj = obj; robj != NULL; robj = robj->next) {
            if ((mask & AOBJ_VISIT_ROBJ) != 0 && robj->aobj != NULL) {
                AObjNotify(callback, value_type, robj->aobj, robj, AOBJ_OBJECT_KIND_ROBJ, value);
            }
        }
        break;
    }
    case AOBJ_OBJECT_KIND_WOBJ:
        if (obj != NULL) {
            HSD_RObj* robj;
            if ((mask & AOBJ_VISIT_WOBJ) != 0 && ((HSD_WObj*) obj)->aobj != NULL) {
                AObjNotify(callback, value_type, ((HSD_WObj*) obj)->aobj, obj, AOBJ_OBJECT_KIND_WOBJ, value);
            }
            for (robj = ((HSD_WObj*) obj)->robj; robj != NULL; robj = robj->next) {
                if ((mask & AOBJ_VISIT_ROBJ) != 0 && robj->aobj != NULL) {
                    AObjNotify(callback, value_type, robj->aobj, robj, AOBJ_OBJECT_KIND_ROBJ, value);
                }
            }
        }
        break;
    case AOBJ_OBJECT_KIND_FOG:
        if ((mask & AOBJ_VISIT_FOG) != 0 && obj != NULL && ((HSD_Fog*) obj)->aobj != NULL) {
            AObjNotify(callback, value_type, ((HSD_Fog*) obj)->aobj, obj, AOBJ_OBJECT_KIND_FOG, value);
        }
        break;
    default:
        HSD_ERR(862, "unexpected type of object.\n");
        break;
    }
}

static void AObjWalkJObj(HSD_JObj* obj, u32 mask, HSD_AObjCallback cb,
                         AObjNotifyFormat fmt, AObjValue* value)
{
    HSD_JObj* child;
    HSD_RObj* robj;
    BOOL has_dobj;

    HSD_ASSERT(715, obj);

    if ((mask & AOBJ_VISIT_JOBJ) != 0 && obj->aobj != NULL) {
        AObjNotify(cb, fmt, obj->aobj, obj, AOBJ_OBJECT_KIND_JOBJ, *value);
    }

    has_dobj = (obj->flags & (JOBJ_SPLINE | JOBJ_PTCL)) == 0;
    if (has_dobj) {
        AObjWalkDObj(obj->u.dobj, mask, cb, fmt, value);
    }

    for (robj = obj->robj; robj != NULL; robj = robj->next) {
        if ((mask & AOBJ_VISIT_ROBJ) != 0 && robj->aobj != NULL) {
            AObjNotify(cb, fmt, robj->aobj, robj, AOBJ_OBJECT_KIND_ROBJ, *value);
        }
    }

    if ((obj->flags & JOBJ_INSTANCE) == 0) {
        for (child = obj->child; child != NULL; child = child->next) {
            AObjWalkJObj(child, mask, cb, fmt, value);
        }
    }
}

static void AObjWalkDObj(HSD_DObj* dobj, u32 mask, HSD_AObjCallback cb,
                         AObjNotifyFormat fmt, AObjValue* value)
{
    HSD_MObj* mobj;
    HSD_PObj* pobj;
    HSD_TObj* tobj;

    for (; dobj != NULL; dobj = dobj->next) {
        if ((mask & AOBJ_VISIT_DOBJ) != 0 && dobj->aobj != NULL) {
            AObjNotify(cb, fmt, dobj->aobj, dobj, AOBJ_OBJECT_KIND_DOBJ, *value);
        }

        mobj = dobj->mobj;
        if (mobj != NULL) {
            if ((mask & AOBJ_VISIT_MOBJ) != 0 && mobj->aobj != NULL) {
                AObjNotify(cb, fmt, mobj->aobj, mobj, AOBJ_OBJECT_KIND_MOBJ, *value);
            }
            for (tobj = mobj->tobj; tobj != NULL; tobj = tobj->next) {
                if ((mask & AOBJ_VISIT_TOBJ) != 0 && tobj->aobj != NULL) {
                    AObjNotify(cb, fmt, tobj->aobj, tobj, AOBJ_OBJECT_KIND_TOBJ, *value);
                }
            }
        }

        for (pobj = dobj->pobj; pobj != NULL; pobj = pobj->next) {
            if ((mask & AOBJ_VISIT_POBJ) != 0 && pobj->aobj != NULL) {
                AObjNotify(cb, fmt, pobj->aobj, pobj, AOBJ_OBJECT_KIND_POBJ, *value);
            }
        }
    }
}

static inline void AObjWalkWObj(HSD_WObj* wobj, u32 mask, HSD_AObjCallback cb,
                                AObjNotifyFormat fmt, AObjValue* value)
{
    HSD_RObj* robj;

    if (wobj != NULL) {
        if ((mask & AOBJ_VISIT_WOBJ) != 0 && wobj->aobj != NULL) {
            AObjNotify(cb, fmt, wobj->aobj, wobj, AOBJ_OBJECT_KIND_WOBJ, *value);
        }
        for (robj = wobj->robj; robj != NULL; robj = robj->next) {
            if ((mask & AOBJ_VISIT_ROBJ) != 0 && robj->aobj != NULL) {
                AObjNotify(cb, fmt, robj->aobj, robj, AOBJ_OBJECT_KIND_ROBJ, *value);
            }
        }
    }
}

static void AObjWalkLObj(HSD_LObj* lobj, u32 mask, HSD_AObjCallback cb,
                         AObjNotifyFormat fmt, AObjValue* value)
{
    for (; lobj != NULL; lobj = lobj->next) {
        if ((mask & AOBJ_VISIT_LOBJ) != 0 && lobj->aobj != NULL) {
            AObjNotify(cb, fmt, lobj->aobj, lobj, AOBJ_OBJECT_KIND_LOBJ, *value);
        }
        AObjWalkWObj(lobj->position, mask, cb, fmt, value);
        AObjWalkWObj(lobj->interest, mask, cb, fmt, value);
    }
}

static void AObjWalkCObj(HSD_CObj* cobj, u32 mask, HSD_AObjCallback cb,
                         AObjNotifyFormat fmt, AObjValue* value)
{
    if (cobj == NULL) {
        return;
    }

    if ((mask & AOBJ_VISIT_COBJ) != 0 && cobj->aobj != NULL) {
        AObjNotify(cb, fmt, cobj->aobj, cobj, AOBJ_OBJECT_KIND_COBJ, *value);
    }
    AObjWalkWObj(cobj->eyepos, mask, cb, fmt, value);
    AObjWalkWObj(cobj->interest, mask, cb, fmt, value);
}

static inline void AObjSetFObj(HSD_AObj* aobj, HSD_FObj* fobj)
{
    if (aobj != NULL) {
        if (aobj->fobj != NULL) {
            HSD_FObjRemoveAll(aobj->fobj);
        }
        aobj->fobj = fobj;
    }
}

static inline void AObjSetObject(HSD_AObj* aobj, HSD_JObj* obj)
{
    if (aobj != NULL) {
        if (aobj->obj != NULL) {
            HSD_JObjUnref(aobj->obj);
        }
        aobj->obj = obj;
    }
}

static inline void AObjFree(HSD_AObj* aobj)
{
    if (aobj != NULL) {
        HSD_ObjFree(&hsdAObjAllocData, aobj);
    }
}

void HSD_AObjRelease(HSD_AObj* aobj)
{
    if (aobj != NULL) {
        AObjSetFObj(aobj, NULL);
        AObjSetObject(aobj, NULL);
        AObjFree(aobj);
    }
}

static inline HSD_AObj* AObjAlloc(void)
{
    HSD_AObj* new = HSD_ObjAlloc(&hsdAObjAllocData);
    HSD_ASSERT(489, new);
    memset(new, 0, sizeof(HSD_AObj));
    new->flags = AOBJ_STOPPED;
    new->framerate = 1.0f;
    return new;
}

static inline void AObjSetFlags(HSD_AObj* aobj, u32 flags)
{
    if (aobj != NULL) {
        u32 anim_flags = flags & AOBJ_FRAME_FLAGS;
        aobj->flags |= anim_flags;
    }
}

static inline void AObjStop(HSD_AObj* aobj, void* obj, HSD_AObjUpdateCallback cb)
{
    HSD_FObjStopAll(aobj->fobj, obj, cb, aobj->framerate);
    aobj->flags |= AOBJ_STOPPED;
}

static inline void AObjSetStartFrame(HSD_AObj* aobj, f32 frame)
{
    if (aobj != NULL) {
        aobj->start_frame = frame;
    }
}

static inline void AObjSetEndFrame(HSD_AObj* aobj, f32 frame)
{
    if (aobj != NULL) {
        aobj->end_frame = frame;
    }
}

HSD_AObj* HSD_AObjLoad(HSD_AObjDesc* desc)
{
    HSD_AObj* aobj;
    HSD_FObj* fobj;
    HSD_JObj* obj;

    if (desc != NULL) {
        aobj = AObjAlloc();
        AObjSetFlags(aobj, desc->flags);
        AObjSetStartFrame(aobj, 0.0f);
        AObjSetEndFrame(aobj, desc->frame);
        fobj = HSD_FObjLoadDesc(desc->fobj);
        AObjSetFObj(aobj, fobj);
        if (desc->objdesc != NULL) {
            obj = HSD_IDGet(NULL, (u32) desc->objdesc, NULL);
            if (obj != NULL) {
                HSD_ObjAddRef(HSD_OBJ(obj));
            } else {
                obj = HSD_JObjLoadDesc(desc->objdesc);
            }
            AObjSetObject(aobj, obj);
        }
        return aobj;
    }
    return NULL;
}

void HSD_AObjInterpret(HSD_AObj* aobj, void* obj, HSD_AObjUpdateCallback cb)
{
    f32 step;

    if (aobj == NULL || (aobj->flags & AOBJ_STOPPED)) {
        return;
    }

    if (aobj->flags & AOBJ_FIRST_FRAME) {
        aobj->flags &= ~AOBJ_FIRST_FRAME;
        step = 0.0f;
    } else {
        step = aobj->framerate;
        aobj->curr_frame += step;
    }

    if ((aobj->flags & AOBJ_LOOP) && aobj->end_frame <= aobj->curr_frame) {
        if (aobj->start_frame < aobj->end_frame) {
            HSD_FObjStopAll(aobj->fobj, obj, cb, step);
            aobj->curr_frame = aobj->start_frame + fmod(aobj->curr_frame - aobj->start_frame,
                                                    aobj->end_frame - aobj->start_frame);
            HSD_FObjReqAnimAll(aobj->fobj, aobj->curr_frame);
        } else {
            aobj->curr_frame = aobj->end_frame;
        }
        step = 0.0f;
        aobj->flags |= AOBJ_LOOPED;
    } else {
        aobj->flags &= ~AOBJ_LOOPED;
    }

    if (aobj->flags & AOBJ_NO_UPDATE) {
        HSD_FObjInterpretAnimAll(aobj->fobj, obj, NULL, step);
    } else {
        HSD_FObjInterpretAnimAll(aobj->fobj, obj, cb, step);
    }

    if (!(aobj->flags & AOBJ_LOOP) && aobj->end_frame <= aobj->curr_frame && aobj != NULL) {
        AObjStop(aobj, obj, cb);
    }

    if (aobj->flags & AOBJ_STOPPED) {
        aobjStoppedCount++;
    } else {
        aobjActiveCount++;
    }
}

void HSD_AObjSetFrame(HSD_AObj* aobj, f32 frame)
{
    if (aobj != NULL) {
        aobj->curr_frame = frame;
        aobj->flags = (aobj->flags & ~AOBJ_STOPPED) | AOBJ_FIRST_FRAME;
        HSD_FObjReqAnimAll(aobj->fobj, frame);
    }
}

void HSD_AObjProcessAnimCb(void)
{
    AObjCallbackNode* node;

    if (aobjStoppedCount != 0 && aobjActiveCount == 0) {
        for (node = aobjAnimCallbackList; node != NULL; node = node->next) {
            node->callback();
        }
    }
}

void HSD_AObjResetAnimCb(void)
{
    aobjStoppedCount = 0;
    aobjActiveCount = 0;
}

void HSD_AObjClearFlags(HSD_AObj* aobj, u32 flags)
{
    if (aobj != NULL) {
        flags &= AOBJ_FRAME_FLAGS;
        aobj->flags &= ~flags;
    }
}

void HSD_AObjSetFlags(HSD_AObj* aobj, u32 flags)
{
    if (aobj != NULL) {
        flags &= AOBJ_FRAME_FLAGS;
        aobj->flags |= flags;
    }
}

HSD_ObjAllocData* HSD_AObjGetAllocData(void)
{
    return &hsdAObjAllocData;
}

void HSD_AObjInitAllocData(void)
{
    HSD_ObjAllocInit(&hsdAObjAllocData, sizeof(HSD_AObj), 4);
}


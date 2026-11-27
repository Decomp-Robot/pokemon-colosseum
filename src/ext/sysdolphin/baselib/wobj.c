#include <dolphin/mtx.h>
#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/class.h>
#include <sysdolphin/baselib/debug.h>
#include <sysdolphin/baselib/jobj.h>
#include <sysdolphin/baselib/object.h>
#include <sysdolphin/baselib/robj.h>
#include <sysdolphin/baselib/spline.h>
#include <sysdolphin/baselib/wobj.h>

static HSD_ClassInfo* wobjClassInfo;
HSD_WObjInfo hsd_WObjInfo = { { HSD_WObjInit } };

static inline void WObjResolvePosition(HSD_WObj* wobj)
{
    HSD_JObj* jp;

    if (wobj->flags & WOBJ_FLAG_LOCAL_POS) {
        if (wobj->aobj != NULL && wobj->aobj->obj != NULL) {
            jp = (HSD_JObj*) wobj->aobj->obj;
            HSD_JObjSetupMatrix(jp);
            PSMTXMultVec(jp->mtx, &wobj->pos, &wobj->pos);
        }
        wobj->flags &= ~WOBJ_FLAG_LOCAL_POS;
    }
}

static inline void WObjSetPositionX(HSD_WObj* wobj, f32 value)
{
    if (wobj == NULL) {
        return;
    }
    WObjResolvePosition(wobj);
    wobj->pos.x = value;
    wobj->flags |= WOBJ_FLAG_HAS_POS;
}

static inline void WObjSetPositionY(HSD_WObj* wobj, f32 value)
{
    if (wobj == NULL) {
        return;
    }
    WObjResolvePosition(wobj);
    wobj->pos.y = value;
    wobj->flags |= WOBJ_FLAG_HAS_POS;
}

static inline void WObjSetPositionZ(HSD_WObj* wobj, f32 value)
{
    if (wobj == NULL) {
        return;
    }
    WObjResolvePosition(wobj);
    wobj->pos.z = value;
    wobj->flags |= WOBJ_FLAG_HAS_POS;
}

void HSD_WObjInit(void)
{
    HSD_ClassInfoInit(&hsd_WObjInfo.parent, &hsd_ObjInfo, "sysdolphin_base_library", "had_wobj",
                      sizeof(HSD_WObjInfo), sizeof(HSD_WObj));
    hsd_WObjInfo.parent.release = &HSD_WObjRelease;
    hsd_WObjInfo.parent.reset = &HSD_WObjAmnesia;
    hsd_WObjInfo.load = &HSD_WObjLoadDesc;
    hsd_WObjInfo.update = &HSD_WObjUpdate;
}

void HSD_WObjAmnesia(HSD_ClassInfo* info)
{
    if (info == wobjClassInfo) {
        wobjClassInfo = NULL;
    }
    hsd_WObjInfo.parent.parent->reset(info);
}

void HSD_WObjRelease(void* obj)
{
    HSD_WObj* wobj = obj;

    HSD_RObjRelease(wobj->robj);
    HSD_AObjRelease(wobj->aobj);
    hsd_WObjInfo.parent.parent->release(wobj);
}

HSD_WObj* HSD_WObjAlloc(void)
{
    HSD_WObj* wobj = hsdNew(wobjClassInfo != NULL ? wobjClassInfo : &hsd_WObjInfo.parent);

    HSD_ASSERT(599, wobj);
    return wobj;
}

void HSD_WObjGetPosition(HSD_WObj* wobj, Vec* pos)
{
    if (wobj == NULL || pos == NULL) {
        return;
    }
    WObjResolvePosition(wobj);
    *pos = wobj->pos;
}

void HSD_WObjSetPosition(HSD_WObj* wobj, Vec* pos)
{
    if (wobj == NULL || pos == NULL) {
        return;
    }
    wobj->pos = *pos;
    wobj->flags |= WOBJ_FLAG_HAS_POS;
    wobj->flags &= ~WOBJ_FLAG_LOCAL_POS;
}

HSD_WObj* HSD_WObjLoad(HSD_WObjDesc* desc)
{
    HSD_ClassInfo* info;
    HSD_WObj* wobj;

    if (desc != NULL) {
        if (desc->class_name == NULL || (info = HSD_ClassSearch(desc->class_name)) == NULL) {
            wobj = HSD_WObjAlloc();
        } else {
            wobj = hsdNew(info);
            HSD_ASSERT(260, wobj);
        }
        ((HSD_WObjInfo*) wobj->obj.class_info)->load(wobj, desc);
        return wobj;
    }
    return NULL;
}

void HSD_WObjInitDesc(HSD_WObj* wobj, HSD_WObjDesc* desc)
{
    if (wobj == NULL || desc == NULL) {
        return;
    }
    HSD_WObjSetPosition(wobj, &desc->position);
    if (wobj->robj != NULL) {
        HSD_RObjRelease(wobj->robj);
    }
    wobj->robj = HSD_RObjLoad(desc->robjdesc);
    HSD_RObjResolveRefs(wobj->robj, desc->robjdesc);
}

s32 HSD_WObjLoadDesc(HSD_WObj* wobj, HSD_WObjDesc* desc)
{
    HSD_WObjSetPosition(wobj, &desc->position);
    if (wobj->robj != NULL) {
        HSD_RObjRelease(wobj->robj);
    }
    wobj->robj = HSD_RObjLoad(desc->robjdesc);
    HSD_RObjResolveRefs(wobj->robj, desc->robjdesc);
    return 0;
}

void HSD_WObjInterpretAnim(HSD_WObj* wobj)
{
    if (wobj == NULL) {
        return;
    }
    HSD_AObjInterpret(wobj->aobj, wobj, ((HSD_WObjInfo*) wobj->obj.class_info)->update);
    HSD_RObjInterpretAnim(wobj->robj);
}

void HSD_WObjUpdate(void* obj, s32 cmd, f32* value)
{
    HSD_WObj* wobj = obj;
    HSD_JObj* jp;
    Vec pos;

    if (wobj == NULL) {
        return;
    }
    switch (cmd) {
    case HSD_WOBJ_TRACK_PATH:
        if (*value < 0.0) {
            *value = 0.0f;
        }
        if (1.0 < *value) {
            *value = 1.0f;
        }
        HSD_ASSERT(152, wobj->aobj);
        jp = (HSD_JObj*) wobj->aobj->obj;
        HSD_ASSERT(154, jp);
        HSD_ASSERT(155, jp->u.spline);
        HSD_SplineEvaluate(&pos, jp->u.spline, *value);
        HSD_WObjSetPosition(wobj, &pos);
        wobj->flags |= WOBJ_FLAG_LOCAL_POS;
        break;
    case HSD_WOBJ_TRACK_X:
        WObjSetPositionX(wobj, *value);
        break;
    case HSD_WOBJ_TRACK_Y:
        WObjSetPositionY(wobj, *value);
        break;
    case HSD_WOBJ_TRACK_Z:
        WObjSetPositionZ(wobj, *value);
        break;
    }
}

void HSD_WObjAddAnim(HSD_WObj* wobj, HSD_WObjAnimDesc* desc)
{
    if (wobj == NULL) {
        return;
    }
    if (desc == NULL) {
        return;
    }
    if (wobj->aobj != NULL) {
        HSD_AObjRelease(wobj->aobj);
    }
    wobj->aobj = HSD_AObjLoad(desc->aobjdesc);
    HSD_RObjAddAnim(wobj->robj, desc->robjanimdesc);
}

void HSD_WObjSetFrame(HSD_WObj* wobj, f32 frame)
{
    if (wobj == NULL) {
        return;
    }
    HSD_AObjSetFrame(wobj->aobj, frame);
    HSD_RObjSetFrame(wobj->robj, frame);
}

void HSD_WObjRemoveAnim(HSD_WObj* wobj)
{
    if (wobj == NULL) {
        return;
    }
    HSD_AObjRelease(wobj->aobj);
    wobj->aobj = NULL;
    HSD_RObjRemoveAnim(wobj->robj);
}

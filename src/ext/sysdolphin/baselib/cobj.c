#include <dolphin/mtx.h>
#include <dolphin/types.h>
#include <dolphin/gx/GXCull.h>
#include <dolphin/gx/GXEnum.h>
#include <dolphin/gx/GXStruct.h>
#include <dolphin/gx/GXTransform.h>
#include <dolphin/os.h>
#include <dolphin/vi/vifuncs.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/class.h>
#include <sysdolphin/baselib/cobj.h>
#include <sysdolphin/baselib/debug.h>
#include <sysdolphin/baselib/math.h>
#include <sysdolphin/baselib/mtx.h>
#include <sysdolphin/baselib/object.h>
#include <sysdolphin/baselib/state.h>
#include <sysdolphin/baselib/wobj.h>

void HSD_CObjInit(void)
{
    HSD_ClassInfoInit(&hsdCObj.info, &hsd_ObjInfo, "sysdolphin_base_library", "hsd_cobj",
                      sizeof(HSD_CObjInfo), sizeof(HSD_CObj));
    hsdCObj.info.init_instance = (s32 (*)(void*)) HSD_CObjInitInstance;
    hsdCObj.info.release = (void (*)(void*)) HSD_CObjRelease;
    hsdCObj.info.reset = HSD_CObjReset;
    hsdCObj.load = HSD_CObjLoadData;
    hsdCObj.update = HSD_CObjSetParam;
}

void HSD_CObjReset(HSD_ClassInfo* class_info)
{
    if (class_info == HSD_CObjClass) {
        HSD_CObjClass = NULL;
    }
    if (class_info == &hsdCObj.info) {
        HSD_CObjCurrent = NULL;
    }
    hsdCObj.info.parent->reset(class_info);
}

static inline HSD_WObj* HSD_CObjGetEyePosWObj(HSD_CObj* cobj)
{
    HSD_ASSERT(744, cobj);
    return cobj->eyepos;
}

static inline HSD_WObj* HSD_CObjGetInterestWObj(HSD_CObj* cobj)
{
    HSD_ASSERT(720, cobj);
    return cobj->interest;
}

static inline void HSD_CObjReportIllegalUp(void)
{
    OSReport("illegal up vector.");
    HSD_ASSERT(996, 0);
}

static inline void HSD_CObjSetUpUpVec(HSD_CObj* cobj, Vec* src, Vec* dst)
{
    s32 invalid_up;
    if ((fabs)(src->x) <= MinNormalFloat[0] && (fabs)(src->y) <= MinNormalFloat[0] && (fabs)(src->z) <= MinNormalFloat[0])
        invalid_up = -1;
    else {
        PSVECNormalize(src, dst);
        invalid_up = 0;
    }
    if (invalid_up)
        HSD_CObjReportIllegalUp();
    if (cobj->up.x != dst->x || cobj->up.y != dst->y || cobj->up.z != dst->z) {
        cobj->flags |= COBJ_VIEW_STATE_DIRTY;
        cobj->up = *dst;
    }
}

static inline void HSD_CObjGetEyePosVec(HSD_CObj* cobj, Vec* pos)
{
    HSD_ASSERT(792, cobj);
    HSD_WObjGetPosition(HSD_CObjGetEyePosWObj(cobj), pos);
}

static inline void HSD_CObjGetInterestVec(HSD_CObj* cobj, Vec* pos)
{
    HSD_ASSERT(768, cobj);
    HSD_WObjGetPosition(HSD_CObjGetInterestWObj(cobj), pos);
}

HSD_CObjInfo hsdCObj = { HSD_CObjInit };
Vec HSD_CObjOriginVec = { 0.0f, 0.0f, 0.0f };
Vec HSD_CObjCameraUpVec = { 0.0f, 1.0f, 0.0f };
Vec HSD_CObjDefaultUpVec = { 0.0f, 1.0f, 0.0f };
HSD_CObj* HSD_CObjCurrent;
HSD_ClassInfo* HSD_CObjClass;
s32 HSD_CObjDefaultWidth = 640;
s32 HSD_CObjDefaultHeight = 480;
HSD_CObjViewportFunc HSD_CObjViewportHook[2] = { HSD_CObjDefaultViewportHook };

void HSD_CObjRelease(HSD_CObj* cobj)
{
    HSD_AObjRelease(cobj->aobj);
    HSD_WObjUnref(HSD_CObjGetEyePosWObj(cobj));
    HSD_WObjUnref(HSD_CObjGetInterestWObj(cobj));
    if (cobj->inv_view_mtx != NULL) {
        HSD_MtxFree(cobj->inv_view_mtx);
    }
    hsdCObj.info.parent->release(cobj);
}

static inline void HSD_CObjSetObjFlags(HSD_CObj* cobj, HSD_CObjFlags flags)
{
    if (cobj != NULL) {
        cobj->flags |= flags;
    }
}

static inline void HSD_CObjClearObjFlags(HSD_CObj* cobj, HSD_CObjFlags flags)
{
    if (cobj != NULL) {
        cobj->flags &= ~flags;
    }
}

s32 HSD_CObjInitInstance(HSD_CObj* cobj)
{
    s32 ret = hsdCObj.info.parent->init_instance(cobj);
    if (ret < 0) {
        return ret;
    }
    HSD_CObjSetObjFlags(cobj, COBJ_VIEW_STATE_DIRTY);
    cobj->eyepos = HSD_WObjAlloc();
    cobj->interest = HSD_WObjAlloc();
    return 0;
}

static inline HSD_CObj* AllocCObj(void)
{
    HSD_CObj* cobj = hsdNew(HSD_CObjClass != NULL ? HSD_CObjClass : &hsdCObj.info);
    HSD_ASSERT(1956, cobj);
    return cobj;
}

HSD_CObj* HSD_CObjLoad(HSD_CObjDesc* desc)
{
    HSD_CObj* cobj;
    HSD_ClassInfo* class_info;

    if (desc != NULL) {
        if (desc->class_name == NULL ||
            (class_info = HSD_ClassSearch(desc->class_name)) == NULL)
        {
            cobj = AllocCObj();
        } else {
            cobj = hsdNew(class_info);
            HSD_ASSERT(2041, cobj);
        }
        ((HSD_CObjInfo*) cobj->obj.class_info)->load(cobj, desc);
        return cobj;
    }
    return NULL;
}

static inline void SetFlags(HSD_CObj* cobj, HSD_CObjFlags flags)
{
    if (cobj != NULL) {
        cobj->flags = (cobj->flags & COBJ_VIEW_STATE_DIRTY) | (flags & HSD_OBJ_USER_FLAGS);
    }
}

static inline void SetViewportFromShorts(HSD_CObj* cobj, s16 viewport[4])
{
    if (cobj != NULL) {
        cobj->viewport[0] = viewport[0];
        cobj->viewport[1] = viewport[1];
        cobj->viewport[2] = viewport[2];
        cobj->viewport[3] = viewport[3];
    }
}

static inline void SetScissor(HSD_CObj* cobj, HSD_CObjScissor* scissor)
{
    if (cobj != NULL) {
        cobj->scissor = *scissor;
    }
}

static inline void SetNear(HSD_CObj* cobj, f32 near)
{
    if (cobj != NULL) {
        cobj->near = near;
    }
}

static inline void SetFar(HSD_CObj* cobj, f32 far)
{
    if (cobj != NULL) {
        cobj->far = far;
    }
}

static inline void SetPerspective(HSD_CObj* cobj, f32 fov, f32 aspect)
{
    if (cobj != NULL) {
        cobj->projection_type = COBJ_PROJ_PERSPECTIVE;
        cobj->projection.perspective.fov = fov;
        cobj->projection.perspective.aspect = aspect;
    }
}

static inline void SetFrustum(HSD_CObj* cobj, f32 top, f32 bottom, f32 left,
                              f32 right)
{
    if (cobj != NULL) {
        cobj->projection_type = COBJ_PROJ_FRUSTUM;
        cobj->projection.bounds.top = top;
        cobj->projection.bounds.bottom = bottom;
        cobj->projection.bounds.left = left;
        cobj->projection.bounds.right = right;
    }
}

static inline void SetOrtho(HSD_CObj* cobj, f32 top, f32 bottom, f32 left,
                            f32 right)
{
    if (cobj != NULL) {
        cobj->projection_type = COBJ_PROJ_ORTHO;
        cobj->projection.bounds.top = top;
        cobj->projection.bounds.bottom = bottom;
        cobj->projection.bounds.left = left;
        cobj->projection.bounds.right = right;
    }
}

s32 HSD_CObjLoadData(HSD_CObj* cobj, HSD_CObjDesc* desc)
{
    cobj->flags = desc->flags;
    SetFlags(cobj, desc->flags);
    SetViewportFromShorts(cobj, desc->viewport);
    SetScissor(cobj, &desc->scissor);
    HSD_WObjInitDesc(cobj->eyepos, desc->eyepos);
    HSD_WObjInitDesc(cobj->interest, desc->interest);
    SetNear(cobj, desc->near);
    SetFar(cobj, desc->far);
    if (desc->flags & COBJ_UP_VECTOR) {
        if (desc->up != NULL) {
            HSD_CObjSetUpVec(cobj, desc->up);
        } else {
            HSD_CObjSetUpVec(cobj, &HSD_CObjDefaultUpVec);
        }
    } else {
        HSD_CObjSetUp(cobj, desc->roll);
    }
    switch (desc->projection_type) {
    case COBJ_PROJ_PERSPECTIVE:
        SetPerspective(cobj, desc->params[0], desc->params[1]);
        break;
    case COBJ_PROJ_ORTHO:
        SetOrtho(cobj, desc->params[0], desc->params[1], desc->params[2],
                 desc->params[3]);
        break;
    case COBJ_PROJ_FRUSTUM:
        SetFrustum(cobj, desc->params[0], desc->params[1], desc->params[2],
                   desc->params[3]);
        break;
    default:
        HSD_ASSERT(2002, 0);
        break;
    }
    return 0;
}

HSD_CObj* HSD_CObjNew(void)
{
    return AllocCObj();
}

HSD_CObj* HSD_CObjGetCurrent(void)
{
    return HSD_CObjCurrent;
}

void HSD_CObjGetOrtho(HSD_CObj* cobj, f32* top, f32* bottom, f32* left, f32* right)
{
    if (cobj == NULL || cobj->projection_type != COBJ_PROJ_ORTHO) {
        return;
    }

    if (top != NULL) {
        *top = cobj->projection.bounds.top;
    }
    if (bottom != NULL) {
        *bottom = cobj->projection.bounds.bottom;
    }
    if (left != NULL) {
        *left = cobj->projection.bounds.left;
    }
    if (right != NULL) {
        *right = cobj->projection.bounds.right;
    }
}

void HSD_CObjGetPerspective(HSD_CObj* cobj, f32* fov, f32* aspect)
{
    if (cobj == NULL || cobj->projection_type != COBJ_PROJ_PERSPECTIVE) {
        return;
    }

    if (fov != NULL) {
        *fov = cobj->projection.perspective.fov;
    }
    if (aspect != NULL) {
        *aspect = cobj->projection.perspective.aspect;
    }
}

void HSD_CObjSetOrtho(HSD_CObj* cobj, f32 top, f32 bottom, f32 left, f32 right)
{
    if (cobj == NULL) {
        return;
    }

    cobj->projection_type = COBJ_PROJ_ORTHO;
    cobj->projection.bounds.top = top;
    cobj->projection.bounds.bottom = bottom;
    cobj->projection.bounds.left = left;
    cobj->projection.bounds.right = right;
}

void HSD_CObjSetFrustum(HSD_CObj* cobj, f32 top, f32 bottom, f32 left, f32 right)
{
    if (cobj == NULL) {
        return;
    }

    cobj->projection_type = COBJ_PROJ_FRUSTUM;
    cobj->projection.bounds.top = top;
    cobj->projection.bounds.bottom = bottom;
    cobj->projection.bounds.left = left;
    cobj->projection.bounds.right = right;
}

void HSD_CObjSetPerspective(HSD_CObj* cobj, f32 fov, f32 aspect)
{
    if (cobj == NULL) {
        return;
    }

    cobj->projection_type = COBJ_PROJ_PERSPECTIVE;
    cobj->projection.perspective.fov = fov;
    cobj->projection.perspective.aspect = aspect;
}

void HSD_CObjSetProjectionType(HSD_CObj* cobj, HSD_CObjProjectionType type)
{
    if (cobj != NULL) {
        cobj->projection_type = type;
    }
}

HSD_CObjProjectionType HSD_CObjGetProjectionType(HSD_CObj* cobj)
{
    if (cobj == NULL) {
        return COBJ_PROJ_PERSPECTIVE;
    }

    return cobj->projection_type;
}

void HSD_CObjSetViewport(HSD_CObj* cobj, f32 left, f32 right, f32 top, f32 bottom)
{
    if (cobj == NULL) {
        return;
    }

    cobj->viewport[0] = left;
    cobj->viewport[1] = right;
    cobj->viewport[2] = top;
    cobj->viewport[3] = bottom;
}

void HSD_CObjSetViewportFromShorts(HSD_CObj* cobj, s16 viewport[4])
{
    if (cobj == NULL) {
        return;
    }

    cobj->viewport[0] = viewport[0];
    cobj->viewport[1] = viewport[1];
    cobj->viewport[2] = viewport[2];
    cobj->viewport[3] = viewport[3];
}

void HSD_CObjSetScissor(HSD_CObj* cobj, u16 left, u16 right, u16 top, u16 bottom)
{
    if (cobj == NULL) {
        return;
    }

    cobj->scissor.left = left;
    cobj->scissor.right = right;
    cobj->scissor.top = top;
    cobj->scissor.bottom = bottom;
}

void HSD_CObjSetFar(HSD_CObj* cobj, f32 far)
{
    if (cobj != NULL) {
        cobj->far = far;
    }
}

f32 HSD_CObjGetFar(HSD_CObj* cobj)
{
    if (cobj == NULL) {
        return 0.0f;
    }

    return cobj->far;
}

void HSD_CObjSetNear(HSD_CObj* cobj, f32 near)
{
    if (cobj != NULL) {
        cobj->near = near;
    }
}

f32 HSD_CObjGetNear(HSD_CObj* cobj)
{
    if (cobj == NULL) {
        return 0.0f;
    }

    return cobj->near;
}

f32 HSD_CObjGetRight(HSD_CObj* cobj)
{
    if (cobj == NULL) return 0.0f;
    switch (cobj->projection_type) {
    case COBJ_PROJ_PERSPECTIVE: {
        f32 half_fov_tan = tan(0.5f * (HSD_DEG_TO_RAD_F * cobj->projection.perspective.fov));
        return cobj->near * half_fov_tan * cobj->projection.perspective.aspect;
    }
    case COBJ_PROJ_FRUSTUM:
        return cobj->projection.bounds.right;
    case COBJ_PROJ_ORTHO:
        return cobj->projection.bounds.right;
    default:
        return 0.0f;
    }
}

f32 HSD_CObjGetLeft(HSD_CObj* cobj)
{
    if (cobj == NULL) return 0.0f;
    switch (cobj->projection_type) {
    case COBJ_PROJ_PERSPECTIVE: {
        f32 half_fov_tan = tan(0.5f * (HSD_DEG_TO_RAD_F * cobj->projection.perspective.fov));
        return -cobj->near * half_fov_tan * cobj->projection.perspective.aspect;
    }
    case COBJ_PROJ_FRUSTUM:
        return cobj->projection.bounds.left;
    case COBJ_PROJ_ORTHO:
        return cobj->projection.bounds.left;
    default:
        return 0.0f;
    }
}

f32 HSD_CObjGetBottom(HSD_CObj* cobj)
{
    if (cobj == NULL) return 0.0f;
    switch (cobj->projection_type) {
    case COBJ_PROJ_PERSPECTIVE: {
        f32 half_fov_tan = tan(0.5f * (HSD_DEG_TO_RAD_F * cobj->projection.perspective.fov));
        return -cobj->near * half_fov_tan;
    }
    case COBJ_PROJ_FRUSTUM:
        return cobj->projection.bounds.bottom;
    case COBJ_PROJ_ORTHO:
        return cobj->projection.bounds.bottom;
    default:
        return 0.0f;
    }
}

f32 HSD_CObjGetTop(HSD_CObj* cobj)
{
    if (cobj == NULL) return 0.0f;
    switch (cobj->projection_type) {
    case COBJ_PROJ_PERSPECTIVE: {
        f32 half_fov_tan = tan(0.5f * (HSD_DEG_TO_RAD_F * cobj->projection.perspective.fov));
        return cobj->near * half_fov_tan;
    }
    case COBJ_PROJ_FRUSTUM:
        return cobj->projection.bounds.top;
    case COBJ_PROJ_ORTHO:
        return cobj->projection.bounds.top;
    default:
        return 0.0f;
    }
}

void HSD_CObjSetAspect(HSD_CObj* cobj, f32 aspect)
{
    if (cobj == NULL || cobj->projection_type != COBJ_PROJ_PERSPECTIVE) return;
    cobj->projection.perspective.aspect = aspect;
}

void HSD_CObjSetFov(HSD_CObj* cobj, f32 fov)
{
    if (cobj == NULL || cobj->projection_type != COBJ_PROJ_PERSPECTIVE) return;
    cobj->projection.perspective.fov = fov;
}

void HSD_CObjSetUp(HSD_CObj* cobj, f32 roll)
{
    Vec normalized_up, t, perp, dir, up, interest, eye, v;
    Mtx m;
    s32 valid_dir;

    if (cobj == NULL)
        return;
    if (cobj->flags & COBJ_UP_VECTOR) {
        if (cobj == NULL || cobj->eyepos == NULL || cobj->interest == NULL)
            valid_dir = 0;
        else {
            HSD_CObjGetEyePos(cobj, &eye);
            HSD_CObjGetInterest(cobj, &interest);
            PSVECSubtract(&interest, &eye, &dir);
            valid_dir = !HSD_CObjNormalizeVec(&dir, &dir);
        }
        if (valid_dir) {
            f32 yabs = (fabs)(dir.y);
            if (1.0 - yabs < 0.0001) {
                perp.x = HSD_Sqrtf(dir.y * dir.y + dir.z * dir.z);
                perp.y = dir.y * (-dir.x / perp.x);
                perp.z = dir.z * (-dir.x / perp.x);
            } else {
                perp.y = HSD_Sqrtf(dir.x * dir.x + dir.z * dir.z);
                perp.x = dir.x * (-dir.y / perp.y);
                perp.z = dir.z * (-dir.y / perp.y);
            }
            MTXRotAxisRad(m, &dir, -roll);
            MTXMultVecSR(m, &perp, &t);
            PSVECNormalize(&t, &normalized_up);
        }
        if (cobj != NULL) {
            if (cobj->flags & COBJ_UP_VECTOR) {
                HSD_CObjSetUpUpVec(cobj, &normalized_up, &up);
            } else {
                roll = HSD_CObjCalcRoll(cobj, &normalized_up);
                if (cobj == NULL)
                    return;
                if (cobj->flags & COBJ_UP_VECTOR) {
                    HSD_CObjCalcUpVec(cobj, roll, &v);
                    HSD_CObjSetUpVec(cobj, &v);
                } else {
                    if (cobj->up.x != roll)
                        HSD_CObjInvalidateViewMtx(cobj);
                    cobj->up.x = roll;
                }
            }
        }
    } else {
        if (cobj->up.x != roll)
            cobj->flags |= COBJ_VIEW_STATE_DIRTY;
        cobj->up.x = roll;
    }
}

s32 HSD_CObjNormalizeVec(Vec* src, Vec* dst)
{
    if (!src || !dst) return -1;
    if (__fabs(src->x) <= MinNormalFloat[0] && __fabs(src->y) <= MinNormalFloat[0] && __fabs(src->z) <= MinNormalFloat[0]) return -1;
    PSVECNormalize(src, dst);
    return 0;
}

#pragma dont_inline on
MtxPtr HSD_CObjGetViewMtx(HSD_CObj* cobj)
{
    HSD_CObjUpdateViewMtx(cobj);
    return cobj->view_mtx;
}
#pragma dont_inline reset

MtxPtr HSD_CObjGetInvViewMtx(HSD_CObj* cobj)
{
    if (cobj->flags & COBJ_INV_VIEW_MTX_DIRTY) {
        if (cobj->inv_view_mtx == NULL)
            cobj->inv_view_mtx = HSD_MtxAlloc();
        PSMTXInverse(cobj->view_mtx, cobj->inv_view_mtx);
        HSD_CObjClearObjFlags(cobj, COBJ_INV_VIEW_MTX_DIRTY);
    }
    return cobj->inv_view_mtx;
}

void HSD_CObjCopyViewMtx(HSD_CObj* cobj, MtxPtr dst)
{
    PSMTXCopy(HSD_CObjGetViewMtx(cobj), dst);
}

void HSD_CObjInvalidateViewMtx(HSD_CObj* cobj)
{
    cobj->flags |= COBJ_VIEW_STATE_DIRTY;
}

void HSD_CObjSetUpVec(HSD_CObj* cobj, Vec* up)
{
    Vec t, dir, out, interest, eye;
    Mtx m;
    f32 angle;
    f32 dot;
    f32 dab;
    f32 oy;
    s32 ok;

    if (cobj == NULL || up == NULL)
        return;
    if (cobj->flags & COBJ_UP_VECTOR) {
        if (HSD_CObjNormalizeVec(up, &t))
            HSD_CObjReportIllegalUp();
        if (cobj->up.x == t.x && cobj->up.y == t.y && cobj->up.z == t.z)
            return;
        cobj->flags |= COBJ_VIEW_STATE_DIRTY;
        cobj->up = t;
        return;
    }
    if (cobj == NULL || cobj->eyepos == NULL || cobj->interest == NULL)
        ok = 0;
    else {
        HSD_CObjGetEyePosVec(cobj, &eye);
        HSD_CObjGetInterestVec(cobj, &interest);
        PSVECSubtract(&interest, &eye, &dir);
        ok = !HSD_CObjNormalizeVec(&dir, &dir);
    }
    if (!ok) {
        angle = 0.0f;
    } else {
        dot = PSVECDotProduct(up, &dir);
        dab = __fabs(dot);
        if (1.0f - dab < MinNormalFloat[0]) {
            angle = 0.0f;
        } else {
            MTXLookAt(m, &HSD_CObjOriginVec, &HSD_CObjCameraUpVec, &dir);
            MTXMultVecSR(m, up, &out);
            dot = -out.x;
            oy = out.y;
            if (oy == 0.0f)
                angle = (dot >= 0.0f) ? (f32)(HSD_PI / 2.0) : -(f32)(HSD_PI / 2.0);
            else
                angle = atan2(dot, oy);
        }
    }
    HSD_CObjSetUp(cobj, angle);
}

s32 HSD_CObjGetUp(HSD_CObj* cobj, Vec* up)
{
    if (cobj == NULL || up == NULL) return 0;
    if (cobj->flags & COBJ_UP_VECTOR) {
        *up = cobj->up;
        return 1;
    }
    return HSD_CObjCalcUpVec(cobj, cobj->up.x, up);
}

s32 HSD_CObjCalcUpVec(HSD_CObj* cobj, f32 roll, Vec* up)
{
    Vec dir, perp, t, interest, eye;
    Mtx m;
    s32 ok;

    if (cobj == NULL || cobj->eyepos == NULL || cobj->interest == NULL)
        ok = 0;
    else {
        HSD_CObjGetEyePosVec(cobj, &eye);
        HSD_CObjGetInterestVec(cobj, &interest);
        PSVECSubtract(&interest, &eye, &dir);
        ok = !HSD_CObjNormalizeVec(&dir, &dir);
    }
    if (!ok)
        return 0;
    {
        f32 yabs = __fabs(dir.y);
        if (1.0 - yabs < 0.0001) {
            perp.x = HSD_SqrtfBits(dir.y * dir.y + dir.z * dir.z);
            perp.y = dir.y * (-dir.x / perp.x);
            perp.z = dir.z * (-dir.x / perp.x);
        } else {
            perp.y = HSD_SqrtfBits(dir.x * dir.x + dir.z * dir.z);
            perp.x = dir.x * (-dir.y / perp.y);
            perp.z = dir.z * (-dir.y / perp.y);
        }
        MTXRotAxisRad(m, &dir, -roll);
        MTXMultVecSR(m, &perp, &t);
        PSVECNormalize(&t, up);
    }
    return 1;
}

f32 HSD_CObjCalcRoll(HSD_CObj* cobj, Vec* up)
{
    Vec out, dir, interest, eye;
    Mtx m;
    f32 dot;
    f32 dab;
    f32 oy;
    s32 ok;

    if (cobj == NULL || cobj->eyepos == NULL || cobj->interest == NULL)
        ok = 0;
    else {
        HSD_CObjGetEyePosVec(cobj, &eye);
        HSD_CObjGetInterestVec(cobj, &interest);
        PSVECSubtract(&interest, &eye, &dir);
        ok = !HSD_CObjNormalizeVec(&dir, &dir);
    }
    if (!ok)
        return 0.0f;
    dot = PSVECDotProduct(up, &dir);
    dab = __fabs(dot);
    if (1.0f - dab < MinNormalFloat[0])
        return 0.0f;
    MTXLookAt(m, &HSD_CObjOriginVec, &HSD_CObjCameraUpVec, &dir);
    MTXMultVecSR(m, up, &out);
    dot = -out.x;
    oy = out.y;
    if (oy == 0.0f)
        return (dot >= 0.0f) ? (f32)(HSD_PI / 2.0) : -(f32)(HSD_PI / 2.0);
    return atan2(dot, oy);
}

f32 HSD_CObjGetEyeInterestDistance(HSD_CObj* cobj)
{
    Vec eye, interest, dir;

    if (cobj == NULL)
        return 0.0f;
    HSD_ASSERT(851, cobj->eyepos);
    HSD_ASSERT(852, cobj->interest);
    HSD_CObjGetEyePosVec(cobj, &eye);
    HSD_CObjGetInterestVec(cobj, &interest);
    PSVECSubtract(&interest, &eye, &dir);
    return VECMag(&dir);
}

void HSD_CObjSetEyePos(HSD_CObj* cobj, Vec* pos)
{
    HSD_ASSERT(804, cobj);
    HSD_WObjSetPosition(HSD_CObjGetEyePosWObj(cobj), pos);
}

void HSD_CObjGetEyePos(HSD_CObj* cobj, Vec* pos)
{
    HSD_CObjGetEyePosVec(cobj, pos);
}

void HSD_CObjSetInterest(HSD_CObj* cobj, Vec* pos)
{
    HSD_ASSERT(780, cobj);
    HSD_WObjSetPosition(HSD_CObjGetInterestWObj(cobj), pos);
}

void HSD_CObjGetInterest(HSD_CObj* cobj, Vec* pos)
{
    HSD_CObjGetInterestVec(cobj, pos);
}

void HSD_CObjResetRenderState(void)
{
    HSD_RenderStateFlush();
    HSD_RenderStateUpdate();
}

static inline GXProjectionType makeProjection(HSD_CObj* cobj, Mtx44 proj)
{
    GXProjectionType type;

    switch (cobj->projection_type) {
    case COBJ_PROJ_PERSPECTIVE:
        type = GX_PERSPECTIVE;
        MTXPerspective(proj, cobj->projection.perspective.fov, cobj->projection.perspective.aspect, cobj->near, cobj->far);
        break;
    case COBJ_PROJ_FRUSTUM:
        type = GX_PERSPECTIVE;
        MTXFrustum(proj, cobj->projection.bounds.top, cobj->projection.bounds.bottom, cobj->projection.bounds.left,
                   cobj->projection.bounds.right, cobj->near, cobj->far);
        break;
    case COBJ_PROJ_ORTHO:
        type = GX_ORTHOGRAPHIC;
        MTXOrtho(proj, cobj->projection.bounds.top, cobj->projection.bounds.bottom, cobj->projection.bounds.left,
                 cobj->projection.bounds.right, cobj->near, cobj->far);
        break;
    }
    return type;
}

static inline s32 setupNormalCamera(HSD_CObj* cobj)
{
    GXProjectionType type;
    Mtx44 proj;

    HSD_CObjViewportHook[0](0, cobj->viewport[0], cobj->viewport[2],
                         cobj->viewport[1] - cobj->viewport[0],
                         cobj->viewport[3] - cobj->viewport[2], 0.0f, 1.0f);
    GXSetScissor(cobj->scissor.left, cobj->scissor.top,
                 cobj->scissor.right - cobj->scissor.left,
                 cobj->scissor.bottom - cobj->scissor.top);

    type = makeProjection(cobj, proj);
    GXSetProjection(proj, type);
    return 1;
}

static inline s32 setupScaledCamera(HSD_CObj* cobj)
{
    GXRenderModeObj* rmode = &RenderMode;
    f64 x_scale, y_scale;
    f32 top, bottom, left, right;
    f32 width, height;
    GXProjectionType type;
    Mtx44 proj;

    x_scale = (f64) rmode->fbWidth / HSD_CObjDefaultWidth;
    y_scale = (f64) rmode->efbHeight / HSD_CObjDefaultHeight;

    left = cobj->viewport[0] * x_scale;
    right = cobj->viewport[1] * x_scale;
    top = cobj->viewport[2] * y_scale;
    bottom = cobj->viewport[3] * y_scale;
    width = right - left;
    height = bottom - top;
    if (rmode->field_rendering) {
        HSD_CObjViewportHook[0](1, left, top, width, height, 0.0f, 1.0f);
    } else {
        HSD_CObjViewportHook[0](0, left, top, width, height, 0.0f, 1.0f);
    }

    left = cobj->scissor.left * x_scale;
    right = cobj->scissor.right * x_scale;
    top = cobj->scissor.top * y_scale;
    bottom = cobj->scissor.bottom * y_scale;
    width = right - left;
    height = bottom - top;
    GXSetScissor(left, top, width, height);

    type = makeProjection(cobj, proj);
    GXSetProjection(proj, type);
    return 1;
}

s32 HSD_CObjSetupRender(HSD_CObj* cobj)
{
    HSD_RenderPass pass;
    s32 result;

    if (cobj == NULL) {
        return 0;
    }

    pass = HSD_RenderPassGet();
    HSD_RenderStateClear();
    HSD_CObjCurrent = cobj;

    switch (pass) {
    case HSD_RENDER_PASS_NORMAL:
        result = setupNormalCamera(cobj);
        break;
    case HSD_RENDER_PASS_SCALED:
        result = setupScaledCamera(cobj);
        break;
    case HSD_RENDER_PASS_DIRECT:
        result = HSD_CObjSetupClippedViewport(cobj);
        break;
    case HSD_RENDER_PASS_PROJECTION_SCALED:
        result = HSD_CObjSetupClippedScissor(cobj);
        break;
    default:
        HSD_ERR(683, "unkown type of render pass.\n");
        return 0;
    }

    if (!result) {
        return 0;
    }
    HSD_CObjUpdateViewMtx(cobj);
    return 1;
}

static inline s32 HSD_CObjEyeIsDirty(HSD_CObj* cobj)
{
    return cobj->eyepos != NULL && (cobj->eyepos->flags & WOBJ_FLAG_HAS_POS);
}

static inline s32 HSD_CObjInterestIsDirty(HSD_CObj* cobj)
{
    return cobj->interest != NULL && (cobj->interest->flags & WOBJ_FLAG_HAS_POS);
}

static inline s32 HSD_CObjViewMtxIsDirty(HSD_CObj* cobj)
{
    return (cobj->flags & COBJ_VIEW_MTX_DIRTY) || HSD_CObjEyeIsDirty(cobj) ||
           HSD_CObjInterestIsDirty(cobj);
}

void HSD_CObjUpdateViewMtx(HSD_CObj* cobj)
{
    Vec eye;
    Vec up;
    Vec interest;

    if (!(cobj->flags & COBJ_VIEW_MTX_FIXED) && HSD_CObjViewMtxIsDirty(cobj)) {
        HSD_CObjGetEyePos(cobj, &eye);
        if (!HSD_CObjGetUp(cobj, &up)) {
            up.x = 0.0f;
            up.y = 1.0f;
            up.z = 0.0f;
        }
        HSD_CObjGetInterest(cobj, &interest);
        MTXLookAt(cobj->view_mtx, &eye, &up, &interest);
        cobj->eyepos->flags &= ~WOBJ_FLAG_HAS_POS;
        cobj->interest->flags &= ~WOBJ_FLAG_HAS_POS;
        HSD_CObjClearObjFlags(cobj, COBJ_VIEW_MTX_DIRTY);
        HSD_CObjSetObjFlags(cobj, COBJ_INV_VIEW_MTX_DIRTY);
    }
}

s32 HSD_CObjSetupClippedScissor(HSD_CObj* cobj)
{
    GXRenderModeObj* rmode = &RenderMode;
    f32 top, bottom, left, right;
    f32 width, height;
    f32 ratio;
    f32 frustum_top, frustum_bottom, frustum_right;
    Mtx44 proj;
    GXProjectionType type;
    u32 offset = rmode->efbHeight - 8;

    if (cobj->viewport[3] < offset) {
        return 0;
    }

    left = cobj->scissor.left;
    right = cobj->scissor.right;
    top = ((cobj->scissor.top > offset) ? cobj->scissor.top : offset) - offset;
    bottom = cobj->scissor.bottom - offset;
    width = right - left;
    height = bottom - top;
    GXSetScissor(left, top, width, height);

    left = cobj->viewport[0];
    right = cobj->viewport[1];
    top = ((cobj->viewport[2] > offset) ? cobj->viewport[2] : offset) - offset;
    bottom = cobj->viewport[3] - offset;
    width = right - left;
    height = bottom - top;
    ratio = height / (cobj->viewport[3] - cobj->viewport[2]);
    HSD_CObjViewportHook[0](0, left, top, width, height, 0.0f, 1.0f);

    switch (cobj->projection_type) {
    case COBJ_PROJ_PERSPECTIVE:
        type = GX_PERSPECTIVE;
        frustum_bottom = cobj->near * (f32) tan((f32) (HSD_DEG_TO_RAD_F * (0.5 * cobj->projection.perspective.fov)));
        frustum_right = frustum_bottom * cobj->projection.perspective.aspect;
        frustum_top = frustum_bottom * (-1.0f + 2.0f * ratio);
        MTXFrustum(proj, frustum_top, -frustum_bottom, -frustum_right, frustum_right, cobj->near, cobj->far);
        break;
    case COBJ_PROJ_FRUSTUM:
        type = GX_PERSPECTIVE;
        frustum_bottom = cobj->projection.bounds.bottom;
        frustum_top = frustum_bottom + ratio * (cobj->projection.bounds.top - frustum_bottom);
        MTXFrustum(proj, frustum_top, frustum_bottom, cobj->projection.bounds.left, cobj->projection.bounds.right,
                   cobj->near, cobj->far);
        break;
    case COBJ_PROJ_ORTHO:
        type = GX_ORTHOGRAPHIC;
        frustum_bottom = cobj->projection.bounds.bottom;
        frustum_top = frustum_bottom + ratio * (cobj->projection.bounds.top - frustum_bottom);
        MTXOrtho(proj, frustum_top, frustum_bottom, cobj->projection.bounds.left, cobj->projection.bounds.right,
                 cobj->near, cobj->far);
        break;
    }
    GXSetProjection(proj, type);
    return 1;
}

s32 HSD_CObjSetupClippedViewport(HSD_CObj* cobj)
{
    GXRenderModeObj* rmode = &RenderMode;
    f32 top, bottom, left, right;
    f32 width, height;
    f32 ratio;
    f32 frustum_top, frustum_bottom, frustum_right;
    Mtx44 proj;
    GXProjectionType type;

    if (cobj->viewport[2] >= rmode->efbHeight) {
        return 0;
    }

    left = cobj->viewport[0];
    right = cobj->viewport[1];
    top = cobj->viewport[2];
    bottom = (cobj->viewport[3] < rmode->efbHeight) ? cobj->viewport[3] : rmode->efbHeight;
    width = right - left;
    height = bottom - top;
    GXSetScissor(left, top, width, height);

    left = cobj->viewport[0];
    right = cobj->viewport[1];
    top = cobj->viewport[2];
    bottom = (cobj->viewport[3] < rmode->efbHeight) ? cobj->viewport[3] : rmode->efbHeight;
    width = right - left;
    height = bottom - top;
    ratio = height / (cobj->viewport[3] - cobj->viewport[2]);
    HSD_CObjViewportHook[0](0, left, top, width, height, 0.0f, 1.0f);

    switch (cobj->projection_type) {
    case COBJ_PROJ_PERSPECTIVE:
        type = GX_PERSPECTIVE;
        frustum_top = cobj->near * (f32) tan((f32) (HSD_DEG_TO_RAD_F * (0.5 * cobj->projection.perspective.fov)));
        frustum_right = frustum_top * cobj->projection.perspective.aspect;
        frustum_bottom = frustum_top * (1.0f - 2.0f * ratio);
        MTXFrustum(proj, frustum_top, frustum_bottom, -frustum_right, frustum_right, cobj->near, cobj->far);
        break;
    case COBJ_PROJ_FRUSTUM:
        type = GX_PERSPECTIVE;
        frustum_top = cobj->projection.bounds.top;
        frustum_bottom = frustum_top - ratio * (frustum_top - cobj->projection.bounds.bottom);
        MTXFrustum(proj, frustum_top, frustum_bottom, cobj->projection.bounds.left, cobj->projection.bounds.right,
                   cobj->near, cobj->far);
        break;
    case COBJ_PROJ_ORTHO:
        type = GX_ORTHOGRAPHIC;
        frustum_top = cobj->projection.bounds.top;
        frustum_bottom = frustum_top - ratio * (frustum_top - cobj->projection.bounds.bottom);
        MTXOrtho(proj, frustum_top, frustum_bottom, cobj->projection.bounds.left, cobj->projection.bounds.right,
                 cobj->near, cobj->far);
        break;
    }
    GXSetProjection(proj, type);
    return 1;
}

static inline void HSD_CObjSetFrame(HSD_CObj* cobj, f32 frame)
{
    if (cobj != NULL) {
        HSD_AObjSetFrame(cobj->aobj, frame);
        HSD_WObjSetFrame(cobj->eyepos, frame);
        HSD_WObjSetFrame(cobj->interest, frame);
    }
}

void HSD_CObjReqAnim(HSD_CObj* cobj, f32 value)
{
    if (cobj != NULL) {
        HSD_CObjSetFrame(cobj, value);
    }
}

void HSD_CObjUpdateAnim(HSD_CObj* cobj)
{
    if (cobj != NULL) {
        HSD_AObjInterpret(cobj->aobj, cobj, ((HSD_CObjInfo*) cobj->obj.class_info)->update);
        HSD_WObjInterpretAnim(cobj->eyepos);
        HSD_WObjInterpretAnim(cobj->interest);
    }
}

void HSD_CObjSetParam(void* obj, s32 type, f32* val)
{
    HSD_CObj* cobj = obj;
    Vec pos;

    if (cobj == NULL)
        return;

    switch (type) {
    case COBJ_TRACK_EYE_X:
        HSD_CObjGetEyePos(cobj, &pos);
        pos.x = *val;
        HSD_CObjSetEyePos(cobj, &pos);
        break;
    case COBJ_TRACK_EYE_Y:
        HSD_CObjGetEyePos(cobj, &pos);
        pos.y = *val;
        HSD_CObjSetEyePos(cobj, &pos);
        break;
    case COBJ_TRACK_EYE_Z:
        HSD_CObjGetEyePos(cobj, &pos);
        pos.z = *val;
        HSD_CObjSetEyePos(cobj, &pos);
        break;
    case COBJ_TRACK_INTEREST_X:
        HSD_CObjGetInterest(cobj, &pos);
        pos.x = *val;
        HSD_CObjSetInterest(cobj, &pos);
        break;
    case COBJ_TRACK_INTEREST_Y:
        HSD_CObjGetInterest(cobj, &pos);
        pos.x = *val;
        HSD_CObjSetInterest(cobj, &pos);
        break;
    case COBJ_TRACK_INTEREST_Z:
        HSD_CObjGetInterest(cobj, &pos);
        pos.x = *val;
        HSD_CObjSetInterest(cobj, &pos);
        break;
    case COBJ_TRACK_ROLL:
        HSD_CObjSetUp(cobj, *val);
        break;
    case COBJ_TRACK_FOV:
        HSD_CObjSetFov(cobj, *val);
        break;
    case COBJ_TRACK_NEAR:
        HSD_CObjSetNear(cobj, *val);
        break;
    case COBJ_TRACK_FAR:
        HSD_CObjSetFar(cobj, *val);
        break;
    }
}

void HSD_CObjAddAnim(HSD_CObj* cobj, HSD_CObjAnim* anim)
{
    if (cobj != NULL && anim != NULL) {
        if (cobj->aobj != NULL)
            HSD_AObjRelease(cobj->aobj);
        cobj->aobj = HSD_AObjLoad(anim->aobjdesc);
        HSD_WObjAddAnim(HSD_CObjGetEyePosWObj(cobj), anim->eye_anim);
        HSD_WObjAddAnim(HSD_CObjGetInterestWObj(cobj), anim->interest_anim);
    }
}

void HSD_CObjRemoveAnim(HSD_CObj* cobj)
{
    if (cobj == NULL)
        return;

    if (cobj != NULL) {
        HSD_AObjRelease(cobj->aobj);
        cobj->aobj = NULL;
        HSD_WObjRemoveAnim(HSD_CObjGetEyePosWObj(cobj));
        HSD_WObjRemoveAnim(HSD_CObjGetInterestWObj(cobj));
    }
}

void HSD_CObjSetViewportHook(HSD_CObjViewportFunc hook)
{
    if (hook == NULL)
        hook = HSD_CObjDefaultViewportHook;
    HSD_CObjViewportHook[0] = hook;
}

void HSD_CObjDefaultViewportHook(s32 jitter, f32 left, f32 top, f32 width, f32 height, f32 nearz, f32 farz)
{
    if (jitter)
        GXSetViewportJitter(left, top, width, height, nearz, farz, VIGetNextField());
    else
        GXSetViewport(left, top, width, height, nearz, farz);
}

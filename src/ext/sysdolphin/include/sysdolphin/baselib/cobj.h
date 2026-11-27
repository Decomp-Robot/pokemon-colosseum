#ifndef SYSDOLPHIN_BASELIB_COBJ_H
#define SYSDOLPHIN_BASELIB_COBJ_H

#include <dolphin/mtx.h>
#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/class.h>
#include <sysdolphin/baselib/object.h>
#include <sysdolphin/baselib/wobj.h>

typedef struct _HSD_CObj HSD_CObj;
typedef struct _HSD_CObjDesc HSD_CObjDesc;

typedef enum _HSD_CObjProjectionType {
    COBJ_PROJ_PERSPECTIVE = 1,
    COBJ_PROJ_FRUSTUM = 2,
    COBJ_PROJ_ORTHO = 3
} HSD_CObjProjectionType;

typedef enum _HSD_CObjTrack {
    COBJ_TRACK_EYE_X = 1,
    COBJ_TRACK_EYE_Y = 2,
    COBJ_TRACK_EYE_Z = 3,
    COBJ_TRACK_INTEREST_X = 5,
    COBJ_TRACK_INTEREST_Y = 6,
    COBJ_TRACK_INTEREST_Z = 7,
    COBJ_TRACK_ROLL = 9,
    COBJ_TRACK_FOV = 10,
    COBJ_TRACK_NEAR = 11,
    COBJ_TRACK_FAR = 12
} HSD_CObjTrack;

typedef enum _HSD_CObjFlags {
    COBJ_UP_VECTOR = 0x1,
    COBJ_VIEW_MTX_FIXED = 0x2,
    COBJ_VIEW_MTX_DIRTY = 0x40000000,
    COBJ_INV_VIEW_MTX_DIRTY = 0x80000000,
    COBJ_VIEW_STATE_DIRTY = COBJ_VIEW_MTX_DIRTY | COBJ_INV_VIEW_MTX_DIRTY
} HSD_CObjFlags;

typedef struct _HSD_CObjInfo {
    HSD_ClassInfo info;
    s32 (*load)(HSD_CObj* cobj, HSD_CObjDesc* desc);
    void (*update)(void* obj, s32 type, f32* value);
} HSD_CObjInfo;

typedef struct _HSD_CObjAnim {
    HSD_AObjDesc* aobjdesc;
    HSD_WObjAnimDesc* eye_anim;
    HSD_WObjAnimDesc* interest_anim;
} HSD_CObjAnim;

typedef void (*HSD_CObjViewportFunc)(s32, f32, f32, f32, f32, f32, f32);

typedef struct _HSD_CObjScissor {
    u16 left;
    u16 right;
    u16 top;
    u16 bottom;
} HSD_CObjScissor;

struct _HSD_CObj {
    HSD_Obj obj;
    HSD_CObjFlags flags;
    f32 viewport[4];
    HSD_CObjScissor scissor;
    HSD_WObj* eyepos;
    HSD_WObj* interest;
    Vec up;
    f32 near;
    f32 far;
    union {
        struct {
            f32 fov;
            f32 aspect;
        } perspective;
        struct {
            f32 top;
            f32 bottom;
            f32 left;
            f32 right;
        } bounds;
    } projection;
    u8 projection_type;
    Mtx view_mtx;
    HSD_AObj* aobj;
    MtxPtr inv_view_mtx;
};

struct _HSD_CObjDesc {
    char* class_name;
    u16 flags;
    u16 projection_type;
    s16 viewport[4];
    HSD_CObjScissor scissor;
    HSD_WObjDesc* eyepos;
    HSD_WObjDesc* interest;
    f32 roll;
    Vec* up;
    f32 near;
    f32 far;
    f32 params[4];
};

extern HSD_CObjInfo hsdCObj;
extern HSD_ClassInfo* HSD_CObjClass;
extern HSD_CObj* HSD_CObjCurrent;
extern s32 HSD_CObjDefaultWidth;
extern s32 HSD_CObjDefaultHeight;
extern HSD_CObjViewportFunc HSD_CObjViewportHook[2];
extern Vec HSD_CObjOriginVec, HSD_CObjDefaultUpVec, HSD_CObjCameraUpVec;

void HSD_CObjInit(void);
void HSD_CObjReset(HSD_ClassInfo* class_info);
void HSD_CObjRelease(HSD_CObj* cobj);
s32 HSD_CObjInitInstance(HSD_CObj* cobj);
HSD_CObj* HSD_CObjLoad(HSD_CObjDesc* desc);
s32 HSD_CObjLoadData(HSD_CObj* cobj, HSD_CObjDesc* desc);
HSD_CObj* HSD_CObjNew(void);
HSD_CObj* HSD_CObjGetCurrent(void);
void HSD_CObjSetParam(void* obj, s32 type, f32* value);
void HSD_CObjUpdateViewMtx(HSD_CObj* cobj);
MtxPtr HSD_CObjGetViewMtx(HSD_CObj* cobj);
MtxPtr HSD_CObjGetInvViewMtx(HSD_CObj* cobj);
void HSD_CObjCopyViewMtx(HSD_CObj* cobj, MtxPtr dst);
s32 HSD_CObjNormalizeVec(Vec* src, Vec* dst);
s32 HSD_CObjSetupRender(HSD_CObj* cobj);
void HSD_CObjGetEyePos(HSD_CObj* cobj, Vec* pos);
void HSD_CObjGetInterest(HSD_CObj* cobj, Vec* pos);
void HSD_CObjSetEyePos(HSD_CObj* cobj, Vec* pos);
void HSD_CObjSetInterest(HSD_CObj* cobj, Vec* pos);
void HSD_CObjInvalidateViewMtx(HSD_CObj* cobj);
void HSD_CObjSetUp(HSD_CObj* cobj, f32 roll);
void HSD_CObjSetUpVec(HSD_CObj* cobj, Vec* up);
s32 HSD_CObjGetUp(HSD_CObj* cobj, Vec* up);
s32 HSD_CObjCalcUpVec(HSD_CObj* cobj, f32 roll, Vec* up);
f32 HSD_CObjCalcRoll(HSD_CObj* cobj, Vec* up);
f32 HSD_CObjGetEyeInterestDistance(HSD_CObj* cobj);
s32 HSD_CObjSetupClippedScissor(HSD_CObj* cobj);
s32 HSD_CObjSetupClippedViewport(HSD_CObj* cobj);
void HSD_CObjSetViewportHook(HSD_CObjViewportFunc hook);
void HSD_CObjDefaultViewportHook(s32 jitter, f32 left, f32 top, f32 width,
                             f32 height, f32 nearz, f32 farz);
void HSD_CObjReqAnim(HSD_CObj* cobj, f32 frame);
void HSD_CObjUpdateAnim(HSD_CObj* cobj);
void HSD_CObjAddAnim(HSD_CObj* cobj, HSD_CObjAnim* anim);
void HSD_CObjRemoveAnim(HSD_CObj* cobj);
void HSD_CObjSetViewport(HSD_CObj* cobj, f32 left, f32 right, f32 top, f32 bottom);
void HSD_CObjSetViewportFromShorts(HSD_CObj* cobj, s16 viewport[4]);
void HSD_CObjSetScissor(HSD_CObj* cobj, u16 left, u16 right, u16 top, u16 bottom);
void HSD_CObjSetFar(HSD_CObj* cobj, f32 far);
f32 HSD_CObjGetFar(HSD_CObj* cobj);
void HSD_CObjSetNear(HSD_CObj* cobj, f32 near);
f32 HSD_CObjGetNear(HSD_CObj* cobj);
void HSD_CObjSetAspect(HSD_CObj* cobj, f32 aspect);
void HSD_CObjSetFov(HSD_CObj* cobj, f32 fov);
void HSD_CObjSetOrtho(HSD_CObj* cobj, f32 top, f32 bottom, f32 left, f32 right);
void HSD_CObjSetFrustum(HSD_CObj* cobj, f32 top, f32 bottom, f32 left, f32 right);
void HSD_CObjSetPerspective(HSD_CObj* cobj, f32 fov, f32 aspect);
void HSD_CObjGetOrtho(HSD_CObj* cobj, f32* top, f32* bottom, f32* left, f32* right);
void HSD_CObjGetPerspective(HSD_CObj* cobj, f32* fov, f32* aspect);
void HSD_CObjResetRenderState(void);
void HSD_CObjSetProjectionType(HSD_CObj* cobj, HSD_CObjProjectionType type);
HSD_CObjProjectionType HSD_CObjGetProjectionType(HSD_CObj* cobj);
f32 HSD_CObjGetRight(HSD_CObj* cobj);
f32 HSD_CObjGetLeft(HSD_CObj* cobj);
f32 HSD_CObjGetTop(HSD_CObj* cobj);
f32 HSD_CObjGetBottom(HSD_CObj* cobj);

#endif

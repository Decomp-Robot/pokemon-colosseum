#ifndef SYSDOLPHIN_BASELIB_DOBJ_H
#define SYSDOLPHIN_BASELIB_DOBJ_H

#include <dolphin/mtx.h>
#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/class.h>
#include <sysdolphin/baselib/mobj.h>
#include <sysdolphin/baselib/pobj.h>

typedef struct _HSD_DObj HSD_DObj;
typedef struct _HSD_DObjDesc HSD_DObjDesc;

typedef enum _HSD_DObjRenderMode {
    HSD_DOBJ_RENDERMODE_OPAQUE = 2,
    HSD_DOBJ_RENDERMODE_TEXEDGE = 4,
    HSD_DOBJ_RENDERMODE_XLU = 8
} HSD_DObjRenderMode;

typedef enum _HSD_RenderFlag {
    HSD_RENDER_SKIP_MATERIAL = 0x04000000
} HSD_RenderFlag;

typedef struct _HSD_DObjInfo {
    HSD_ClassInfo info;
    void (*disp)(HSD_DObj* dobj, Mtx vmtx, Mtx pmtx, u32 flags);
    s32 (*load)(HSD_DObj* dobj, HSD_DObjDesc* desc);
    HSD_AObjUpdateCallback anim;
} HSD_DObjInfo;

struct _HSD_DObj {
    HSD_Class parent;
    struct _HSD_DObj* next;
    HSD_MObj* mobj;
    HSD_PObj* pobj;
    HSD_AObj* aobj;
    u32 flags;
};

struct _HSD_DObjDesc {
    char* class_name;
    HSD_DObjDesc* next;
    HSD_MObjDesc* mobj_desc;
    HSD_PObjDesc* pobj_desc;
};

void HSD_DObjInit(void);
void HSD_DObjReset(HSD_ClassInfo* class_info);
void HSD_DObjRelease(HSD_DObj* dobj);
void HSD_DObjDisp(HSD_DObj* dobj, Mtx vmtx, Mtx pmtx, u32 flags);
void HSD_DObjGetNbVertexAndPolygon(HSD_DObj* dobj, u32* nb_vertex, u32* nb_polygon);
void HSD_DObjResolveRefsAll(HSD_DObj* dobj, HSD_DObj* other);
void HSD_DObjRemoveAll(HSD_DObj* dobj);
HSD_DObj* HSD_DObjLoadDesc(HSD_DObjDesc* desc);
s32 HSD_DObjLoad(HSD_DObj* obj, HSD_DObjDesc* obj_desc);
void HSD_DObjAnimAll(HSD_DObj* dobj);
void HSD_DObjReqAnimAllByFlags(HSD_DObj* dobj, u32 flags, f32 frame);
void HSD_DObjAddAnimAll(HSD_DObj* dobj, HSD_MObjAnimDesc* mat_anims, HSD_PObjAnimDesc* pobj_anims);
void HSD_DObjSetCurrent(HSD_DObj* dobj);

#endif

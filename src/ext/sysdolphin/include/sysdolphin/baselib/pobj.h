#ifndef SYSDOLPHIN_BASELIB_POBJ_H
#define SYSDOLPHIN_BASELIB_POBJ_H

#include <dolphin/mtx.h>
#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/class.h>

typedef struct _HSD_PObj HSD_PObj;
typedef struct _HSD_PObjDesc HSD_PObjDesc;
typedef struct _HSD_PObjAnim HSD_PObjAnim;
typedef struct _HSD_PObjAnimDesc HSD_PObjAnimDesc;
typedef struct _HSD_PObjInfo HSD_PObjInfo;

typedef struct _HSD_PObj {
    HSD_Class parent;
    struct _HSD_PObj* next;
    u8 unk_08[16];
    HSD_AObj* aobj;
} HSD_PObj;

typedef struct _HSD_PObjInfo {
    HSD_ClassInfo info;
    void (*disp)(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx, u32 flags);
    void (*setup_mtx)(HSD_PObj* pobj, Mtx vmtx, Mtx pmtx, u32 flags);
    void (*load)(HSD_PObj* pobj, HSD_PObjDesc* desc);
    HSD_AObjUpdateCallback anim;
} HSD_PObjInfo;

struct _HSD_PObjAnimDesc {
    HSD_PObjAnimDesc* next;
    HSD_PObjAnim* data;
};

void HSD_PObjGetNbVertexAndPolygon(HSD_PObj* pobj, u32* nb_vertex, u32* nb_polygon);
void HSD_PObjResolveRefsAll(HSD_PObj* pobj, HSD_PObj* other);
void HSD_PObjRemoveAll(HSD_PObj* pobj);
HSD_PObj* HSD_PObjLoadDesc(HSD_PObjDesc* desc);
void HSD_PObjAnimAll(HSD_PObj* pobj);
void HSD_PObjReqAnimAllByFlags(HSD_PObj* pobj, u32 flags, f32 frame);
void HSD_PObjAddAnimAll(HSD_PObj* pobj, HSD_PObjAnim* anim);

#endif

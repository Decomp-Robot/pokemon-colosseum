#ifndef SYSDOLPHIN_BASELIB_MOBJ_H
#define SYSDOLPHIN_BASELIB_MOBJ_H

#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/class.h>
#include <sysdolphin/baselib/object.h>
#include <sysdolphin/baselib/texp.h>
#include <sysdolphin/baselib/tobj.h>

typedef struct _HSD_MObj HSD_MObj;
typedef struct _HSD_MObjDesc HSD_MObjDesc;
typedef struct _HSD_MObjAnimDesc HSD_MObjAnimDesc;
typedef struct _HSD_MObjInfo HSD_MObjInfo;

typedef struct _HSD_MObj {
    HSD_Class parent;
    u32 rendermode;
    HSD_TObj* tobj;
    u8 unk_0C[8];
    HSD_AObj* aobj;
} HSD_MObj;

typedef enum _HSD_MObjBlend {
    HSD_MOBJ_BLEND_NONE = 0,
    HSD_MOBJ_BLEND_TRANSLUCENT = 0x40000000,
    HSD_MOBJ_BLEND_TEXEDGE = 0x60000000
} HSD_MObjBlend;

static const u32 HSD_MOBJ_BLEND_MASK = 0x60000000;

typedef struct _HSD_MObjInfo {
    HSD_ClassInfo info;
    void (*setup)(HSD_MObj* mobj, u32 flags);
    HSD_MObj* (*load)(HSD_MObj* mobj, HSD_MObjDesc* desc);
    HSD_TExp* (*make_texp)(HSD_MObj* mobj, HSD_TObj* textures, HSD_TExp** texp);
    void (*submit_texp)(HSD_MObj* mobj, HSD_TObj* textures, u32 flags);
    HSD_AObjUpdateCallback anim;
    void (*unset)(HSD_MObj* mobj, u32 flags);
} HSD_MObjInfo;

struct _HSD_MObjAnimDesc {
    HSD_MObjAnimDesc* next;
    HSD_AObjDesc* aobj_desc;
    HSD_TObjAnimDesc* tobj_anim;
};

void HSD_MObjRemove(HSD_MObj* mobj);
HSD_MObj* HSD_MObjLoadDesc(HSD_MObjDesc* desc);
void HSD_MObjAnim(HSD_MObj* mobj);
void HSD_MObjReqAnimByFlags(HSD_MObj* mobj, u32 flags, f32 frame);
void HSD_MObjAddAnim(HSD_MObj* mobj, HSD_MObjAnimDesc* desc);
void HSD_MObjSetCurrent(HSD_MObj* mobj);

#endif

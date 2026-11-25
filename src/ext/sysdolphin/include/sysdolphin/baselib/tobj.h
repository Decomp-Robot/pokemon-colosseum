#ifndef SYSDOLPHIN_BASELIB_TOBJ_H
#define SYSDOLPHIN_BASELIB_TOBJ_H

#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/object.h>

typedef struct _HSD_TObjAnimDesc HSD_TObjAnimDesc;

typedef struct _HSD_TObj {
    HSD_Obj parent;
    struct _HSD_TObj* next;
    u8 unk_0C[88];
    HSD_AObj* aobj;
} HSD_TObj;

#endif

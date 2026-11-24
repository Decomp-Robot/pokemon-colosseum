#ifndef SYSDOLPHIN_BASELIB_MOBJ_H
#define SYSDOLPHIN_BASELIB_MOBJ_H

#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/object.h>
#include <sysdolphin/baselib/tobj.h>

typedef struct _HSD_MObj {
    HSD_Obj parent;
    HSD_TObj* tobj;
    u8 unk_0C[8];
    HSD_AObj* aobj;
} HSD_MObj;

#endif

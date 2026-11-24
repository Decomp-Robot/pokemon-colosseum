#ifndef SYSDOLPHIN_BASELIB_COBJ_H
#define SYSDOLPHIN_BASELIB_COBJ_H

#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/object.h>
#include <sysdolphin/baselib/wobj.h>

typedef struct _HSD_CObj {
    HSD_Obj parent;
    u8 unk_08[28];
    HSD_WObj* eye_position;
    HSD_WObj* interest;
    u8 unk_2C[88];
    HSD_AObj* aobj;
} HSD_CObj;

#endif

#ifndef SYSDOLPHIN_BASELIB_LOBJ_H
#define SYSDOLPHIN_BASELIB_LOBJ_H

#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/object.h>
#include <sysdolphin/baselib/wobj.h>

typedef struct _HSD_LObj {
    HSD_Obj parent;
    u8 unk_08[4];
    struct _HSD_LObj* next;
    u8 unk_10[8];
    HSD_WObj* position;
    HSD_WObj* interest;
    u8 unk_20[40];
    HSD_AObj* aobj;
} HSD_LObj;

#endif

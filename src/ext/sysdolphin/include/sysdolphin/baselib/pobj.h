#ifndef SYSDOLPHIN_BASELIB_POBJ_H
#define SYSDOLPHIN_BASELIB_POBJ_H

#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/class.h>

typedef struct _HSD_PObj {
    HSD_Class parent;
    struct _HSD_PObj* next;
    u8 unk_08[16];
    HSD_AObj* aobj;
} HSD_PObj;

#endif

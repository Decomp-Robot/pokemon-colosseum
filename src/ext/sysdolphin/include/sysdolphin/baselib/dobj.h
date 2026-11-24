#ifndef SYSDOLPHIN_BASELIB_DOBJ_H
#define SYSDOLPHIN_BASELIB_DOBJ_H

#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/class.h>
#include <sysdolphin/baselib/mobj.h>
#include <sysdolphin/baselib/pobj.h>

typedef struct _HSD_DObj {
    HSD_Class parent;
    struct _HSD_DObj* next;
    HSD_MObj* mobj;
    HSD_PObj* pobj;
    HSD_AObj* aobj;
} HSD_DObj;

#endif

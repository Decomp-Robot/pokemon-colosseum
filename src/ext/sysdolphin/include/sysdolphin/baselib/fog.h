#ifndef SYSDOLPHIN_BASELIB_FOG_H
#define SYSDOLPHIN_BASELIB_FOG_H

#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/object.h>

typedef struct _HSD_Fog {
    HSD_Obj parent;
    u8 unk_08[20];
    HSD_AObj* aobj;
} HSD_Fog;

#endif

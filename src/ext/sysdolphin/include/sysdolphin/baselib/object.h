#ifndef SYSDOLPHIN_BASELIB_OBJECT_H
#define SYSDOLPHIN_BASELIB_OBJECT_H

#include <dolphin/types.h>

#include <sysdolphin/baselib/class.h>

/* Common prefix shared by the class-based scene objects. */
typedef struct _HSD_Obj {
    HSD_ClassInfo* class_info;
    u16 ref_count;
    u16 ref_count_individual;
} HSD_Obj;

extern HSD_ClassInfo hsd_ObjInfo;

#endif

#ifndef SYSDOLPHIN_BASELIB_OBJECT_H
#define SYSDOLPHIN_BASELIB_OBJECT_H

#include <dolphin/types.h>

#include <sysdolphin/baselib/class.h>
#include <sysdolphin/baselib/debug.h>

typedef struct _HSD_Obj {
    HSD_ClassInfo* class_info;
    u16 ref_count;
    u16 ref_count_individual;
} HSD_Obj;

#define HSD_OBJ(o) ((HSD_Obj*) (o))
#define HSD_OBJ_NOREF 0xFFFF
#define HSD_OBJ_USER_FLAGS 0x3FFFFFFF

static inline void HSD_ObjAddRef(HSD_Obj* o)
{
    if (o != NULL) {
        o->ref_count++;
        HSD_ASSERT(93, HSD_OBJ(o)->ref_count != HSD_OBJ_NOREF);
    }
}

static inline BOOL HSD_ObjUnref(HSD_Obj* obj)
{
    BOOL ret = obj->ref_count == HSD_OBJ_NOREF;
    if (ret) {
        return ret;
    }
    ret = obj->ref_count == 0;
    obj->ref_count--;
    return ret;
}

extern HSD_ClassInfo hsd_ObjInfo;

#endif

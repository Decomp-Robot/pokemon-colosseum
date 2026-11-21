#ifndef SYSDOLPHIN_BASELIB_CLASS_H
#define SYSDOLPHIN_BASELIB_CLASS_H

#include <dolphin/types.h>

typedef struct _HSD_ClassInfo HSD_ClassInfo;

/* Bit in the class descriptor's flags word marking it as initialized. */
#define HSD_CLASS_INITIALIZED 0x1

/* Common prefix of every class descriptor: one-time init routine, flags,
 * identifying names, and the object/descriptor sizes. */
typedef struct _HSD_ClassInfoHead {
    void (*init)(void);
    u32 flags;
    char* library_name;
    char* class_name;
    s16 obj_size;
    s16 info_size;
} HSD_ClassInfoHead;

struct _HSD_ClassInfo {
    HSD_ClassInfoHead head;
    HSD_ClassInfo* parent;
    HSD_ClassInfo* next;
    HSD_ClassInfo* child;
    u32 num_instances;
    u32 num_instances_max;
    /* The method tail below is inherited from the parent. */
    void* (*alloc)(HSD_ClassInfo* class_info);
    s32 (*init_instance)(void* obj);
    void (*release)(void* obj);
    void (*destroy)(void* obj);
    void (*reset)(HSD_ClassInfo* class_info);
};

typedef struct _HSD_Class {
    HSD_ClassInfo* class_info;
} HSD_Class;

extern HSD_ClassInfo hsdClass;

HSD_ClassInfo* HSD_ClassSearch(const char* name);
BOOL hsdIsDescendantOf(HSD_ClassInfo* class_info, HSD_ClassInfo* parent_info);
void* hsdNew(HSD_ClassInfo* class_info);
void HSD_ClassFree(void* ptr, u32 size);
void* HSD_ClassAlloc(u32 size);
void HSD_ClassInfoInit(HSD_ClassInfo* class_info, HSD_ClassInfo* parent_info, const char* library_name, const char* class_name, int info_size, int obj_size);

#endif

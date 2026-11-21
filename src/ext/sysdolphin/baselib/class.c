#include <stddef.h>
#include <string.h>

#include <sysdolphin/baselib/class.h>
#include <sysdolphin/baselib/debug.h>
#include <sysdolphin/baselib/hash.h>
#include <sysdolphin/baselib/memory.h>

static HSD_Hash* class_name_table;
static u32 lbl_8047B224;
static u32 lbl_8047B220;

static void ClassInit(void);
static void ClassAmnesia(HSD_ClassInfo* class_info);
static void ClassDestroy(void* obj);
static void ClassRelease(void* obj);
static s32 ClassInitObj(void* obj);
static void* ClassAlloc(HSD_ClassInfo* class_info);

HSD_ClassInfo hsdClass = { ClassInit };

HSD_ClassInfo* HSD_ClassSearch(const char* name)
{
    if (class_name_table != NULL) {
        return HSD_HashSearch(class_name_table, (void*)name, NULL);
    }
    return NULL;
}

BOOL hsdIsDescendantOf(HSD_ClassInfo* class_info, HSD_ClassInfo* parent_info)
{
    HSD_ClassInfo* info;
    HSD_ClassInfo* parent;

    if (class_info == NULL || parent_info == NULL) {
        return FALSE;
    }
    info = class_info;
    parent = parent_info;
    if (!(info->head.flags & HSD_CLASS_INITIALIZED)) {
        info->head.init();
    }
    if (!(parent->head.flags & HSD_CLASS_INITIALIZED)) {
        parent->head.init();
    }
    while (info != NULL) {
        if (info == parent) {
            return TRUE;
        }
        info = info->parent;
    }
    return FALSE;
}

void* hsdNew(HSD_ClassInfo* class_info)
{
    HSD_ClassInfo* info;
    HSD_ClassInfoHead* head;
    HSD_Class* obj;

    if (!(class_info->head.flags & HSD_CLASS_INITIALIZED)) {
        class_info->head.init();
    }
    obj = class_info->alloc(class_info);
    if (obj == NULL) {
        return NULL;
    }
    info = class_info;
    head = &info->head;
    if (!(head->flags & HSD_CLASS_INITIALIZED)) {
        head->init();
    }
    memset(obj, 0, head->obj_size);
    obj->class_info = class_info;
    if (info->init_instance(obj) < 0) {
        class_info->destroy(obj);
        return NULL;
    }
    return obj;
}

static void ClassInit(void)
{
    hsdClass.head.flags = HSD_CLASS_INITIALIZED;
    hsdClass.head.library_name = "sysdolphin_base_library";
    hsdClass.head.class_name = "hsd_class";
    hsdClass.head.obj_size = sizeof(HSD_Class);
    hsdClass.head.info_size = sizeof(HSD_ClassInfo);
    hsdClass.parent = NULL;
    hsdClass.child = NULL;
    hsdClass.next = NULL;
    hsdClass.num_instances = 0;
    hsdClass.num_instances_max = 0;
    hsdClass.alloc = ClassAlloc;
    hsdClass.init_instance = ClassInitObj;
    hsdClass.release = ClassRelease;
    hsdClass.destroy = ClassDestroy;
    hsdClass.reset = ClassAmnesia;
}

static void ClassAmnesia(HSD_ClassInfo* class_info)
{
    class_info->num_instances = 0;
    class_info->num_instances_max = 0;
    if (class_info == &hsdClass) {
        lbl_8047B224 = 0;
        lbl_8047B220 = 0;
        class_name_table = NULL;
    }
}

static void ClassDestroy(void* obj)
{
    HSD_Class* instance = obj;

    instance->class_info->num_instances--;
    HSD_Free(instance);
}

static void ClassRelease(void* obj)
{
}

static s32 ClassInitObj(void* obj)
{
    return 0;
}

static void* ClassAlloc(HSD_ClassInfo* class_info)
{
    void* obj = HSD_Alloc(class_info->head.obj_size);

    if (obj != NULL) {
        class_info->num_instances++;
        if (class_info->num_instances > class_info->num_instances_max) {
            class_info->num_instances_max = class_info->num_instances;
        }
    }
    return obj;
}

void HSD_ClassFree(void* ptr, u32 size)
{
    HSD_Free(ptr);
}

void* HSD_ClassAlloc(u32 size)
{
    return HSD_Alloc(size);
}

void HSD_ClassInfoInit(HSD_ClassInfo* class_info, HSD_ClassInfo* parent_info, const char* library_name,
                       const char* class_name, int info_size, int obj_size)
{
    class_info->head.flags = HSD_CLASS_INITIALIZED;
    class_info->head.library_name = (char*)library_name;
    class_info->head.class_name = (char*)class_name;
    class_info->head.obj_size = obj_size;
    class_info->head.info_size = info_size;
    class_info->parent = parent_info;
    class_info->child = NULL;
    class_info->next = NULL;
    class_info->num_instances = 0;
    class_info->num_instances_max = 0;

    if (parent_info != NULL) {
        if (!(parent_info->head.flags & HSD_CLASS_INITIALIZED)) {
            parent_info->head.init();
        }
        HSD_ASSERT(103, class_info->head.obj_size >= parent_info->head.obj_size);
        HSD_ASSERT(104, class_info->head.info_size >= parent_info->head.info_size);
        memcpy(&class_info->alloc, &parent_info->alloc, parent_info->head.info_size - offsetof(HSD_ClassInfo, alloc));
        class_info->next = parent_info->child;
        parent_info->child = class_info;
    }
}

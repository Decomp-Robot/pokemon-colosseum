#include <dolphin/os.h>
#include <dolphin/types.h>

#include <sysdolphin/baselib/class.h>
#include <sysdolphin/baselib/debug.h>
#include <sysdolphin/baselib/dobj.h>
#include <sysdolphin/baselib/mobj.h>
#include <sysdolphin/baselib/pobj.h>

static HSD_DObj* current_dobj;
static HSD_ClassInfo* dobj_default_class;

static HSD_DObjInfo hsdDObj = { { HSD_DObjInit } };

static inline HSD_DObj* HSD_DObjAlloc(void)
{
    HSD_DObj* dobj = hsdNew(dobj_default_class != NULL ? dobj_default_class : &hsdDObj.info);
    HSD_ASSERT(532, dobj);
    return dobj;
}

static inline void HSD_DObjSetRenderMode(HSD_DObj* dobj, HSD_DObjRenderMode mode)
{
    if (dobj != NULL) {
        dobj->flags = (dobj->flags & ~(HSD_DOBJ_RENDERMODE_TEXEDGE | HSD_DOBJ_RENDERMODE_XLU | HSD_DOBJ_RENDERMODE_OPAQUE)) | mode;
    }
}

static inline void HSD_DObjResolveRefs(HSD_DObj* dobj, HSD_DObj* other)
{
    if (dobj != NULL && other != NULL) {
        HSD_PObjResolveRefsAll(dobj->pobj, other->pobj);
    }
}

static inline void HSD_DObjRemove(HSD_DObj* dobj)
{
    if (dobj != NULL) {
        dobj->parent.class_info->release(dobj);
        dobj->parent.class_info->destroy(dobj);
    }
}

static inline void HSD_DObjAnim(HSD_DObj* dobj)
{
    if (dobj != NULL) {
        HSD_AObjInterpret(dobj->aobj, dobj, ((HSD_DObjInfo*)dobj->parent.class_info)->anim);
        HSD_PObjAnimAll(dobj->pobj);
        HSD_MObjAnim(dobj->mobj);
    }
}

static inline void HSD_DObjReqAnim(HSD_DObj* dobj, u32 flags, f32 frame)
{
    if (dobj != NULL) {
        HSD_PObjReqAnimAllByFlags(dobj->pobj, flags, frame);
        HSD_MObjReqAnimByFlags(dobj->mobj, flags, frame);
    }
}

static inline void HSD_DObjAddAnim(HSD_DObj* dobj, HSD_MObjAnimDesc* mat_anims, HSD_PObjAnimDesc* pobj_anims)
{
    if (dobj != NULL) {
        HSD_PObjAddAnimAll(dobj->pobj, pobj_anims != NULL ? pobj_anims->data : NULL);
        HSD_MObjAddAnim(dobj->mobj, mat_anims);
    }
}

void HSD_DObjInit(void)
{
    HSD_ClassInfoInit(&hsdDObj.info, &hsdClass, "sysdolphin_base_library", "hsd_dobj",
                      sizeof(HSD_DObjInfo), sizeof(HSD_DObj));
    hsdDObj.info.release = (void (*)(void*))HSD_DObjRelease;
    hsdDObj.info.reset = HSD_DObjReset;
    hsdDObj.disp = HSD_DObjDisp;
    hsdDObj.load = HSD_DObjLoad;
}

void HSD_DObjReset(HSD_ClassInfo* class_info)
{
    if (class_info == dobj_default_class) {
        dobj_default_class = NULL;
    }
    hsdDObj.info.parent->reset(class_info);
}

void HSD_DObjRelease(HSD_DObj* dobj)
{
    HSD_MObjRemove(dobj->mobj);
    HSD_PObjRemoveAll(dobj->pobj);
    HSD_AObjRelease(dobj->aobj);
    hsdDObj.info.parent->release(dobj);
}

void HSD_DObjDisp(HSD_DObj* dobj, Mtx vmtx, Mtx pmtx, u32 flags)
{
    HSD_PObj* pobj;

    HSD_MObjSetCurrent(dobj->mobj);
    if ((flags & HSD_RENDER_SKIP_MATERIAL) == 0) {
        ((HSD_MObjInfo*)HSD_OBJ(dobj->mobj)->class_info)->setup(dobj->mobj, flags);
    }
    pobj = dobj->pobj;
    while (pobj != NULL) {
        ((HSD_PObjInfo*)pobj->parent.class_info)->disp(pobj, vmtx, pmtx, flags);
        pobj = pobj->next;
    }
    if ((flags & HSD_RENDER_SKIP_MATERIAL) == 0) {
        ((HSD_MObjInfo*)HSD_OBJ(dobj->mobj)->class_info)->unset(dobj->mobj, flags);
    }
    HSD_MObjSetCurrent(NULL);
}

void HSD_DObjGetNbVertexAndPolygon(HSD_DObj* dobj, u32* nb_vertex, u32* nb_polygon)
{
    u32 total_vertex = 0;
    u32 total_polygon = 0;
    u32 vertex_count;
    u32 polygon_count;

    while (dobj != NULL) {
        HSD_PObjGetNbVertexAndPolygon(dobj->pobj, &vertex_count, &polygon_count);
        total_vertex += vertex_count;
        total_polygon += polygon_count;
        dobj = dobj->next;
    }
    if (nb_vertex != NULL) {
        *nb_vertex = total_vertex;
    }
    if (nb_polygon != NULL) {
        *nb_polygon = total_polygon;
    }
}

void HSD_DObjResolveRefsAll(HSD_DObj* dobj, HSD_DObj* other)
{
    while (dobj != NULL && other != NULL) {
        HSD_DObjResolveRefs(dobj, other);
        dobj = dobj->next;
        other = other->next;
    }
}

void HSD_DObjRemoveAll(HSD_DObj* dobj)
{
    HSD_DObj* next;

    while (dobj != NULL) {
        next = dobj->next;
        HSD_DObjRemove(dobj);
        dobj = next;
    }
}

HSD_DObj* HSD_DObjLoadDesc(HSD_DObjDesc* desc)
{
    HSD_ClassInfo* class_info;
    HSD_DObj* dobj;

    if (desc == NULL) {
        return NULL;
    }
    if (desc->class_name == NULL || (class_info = HSD_ClassSearch(desc->class_name)) == NULL) {
        dobj = HSD_DObjAlloc();
    } else {
        dobj = hsdNew(class_info);
        HSD_ASSERT(385, dobj);
    }
    ((HSD_DObjInfo*)dobj->parent.class_info)->load(dobj, desc);
    return dobj;
}

s32 HSD_DObjLoad(HSD_DObj* dobj, HSD_DObjDesc* desc)
{
    HSD_DObjDesc* obj_desc = desc;
    HSD_DObj* obj = dobj;

    obj->next = HSD_DObjLoadDesc(obj_desc->next);
    obj->mobj = HSD_MObjLoadDesc(obj_desc->mobj_desc);
    obj->pobj = HSD_PObjLoadDesc(obj_desc->pobj_desc);
    if (obj->mobj != NULL) {
        switch (obj->mobj->rendermode & HSD_MOBJ_BLEND_MASK) {
        case HSD_MOBJ_BLEND_NONE:
            HSD_DObjSetRenderMode(obj, HSD_DOBJ_RENDERMODE_OPAQUE);
            break;
        case HSD_MOBJ_BLEND_TRANSLUCENT:
            HSD_DObjSetRenderMode(obj, HSD_DOBJ_RENDERMODE_XLU);
            break;
        case HSD_MOBJ_BLEND_TEXEDGE:
            HSD_DObjSetRenderMode(obj, HSD_DOBJ_RENDERMODE_TEXEDGE);
            break;
        default:
            OSReport("mobj has unexpected blending flags (0x%x).", obj->mobj->rendermode);
            HSD_ERR(319, "");
            break;
        }
    }
    return 0;
}

void HSD_DObjAnimAll(HSD_DObj* dobj)
{
    HSD_DObj* cur;

    if (dobj == NULL) {
        return;
    }
    cur = dobj;
    while (cur != NULL) {
        HSD_DObjAnim(cur);
        cur = cur->next;
    }
}

void HSD_DObjReqAnimAllByFlags(HSD_DObj* dobj, u32 flags, f32 frame)
{
    HSD_DObj* cur;

    if (dobj == NULL) {
        return;
    }
    cur = dobj;
    while (cur != NULL) {
        HSD_DObjReqAnim(cur, flags, frame);
        cur = cur->next;
    }
}

void HSD_DObjAddAnimAll(HSD_DObj* dobj, HSD_MObjAnimDesc* mat_anims, HSD_PObjAnimDesc* pobj_anims)
{
    HSD_DObj* cur;
    HSD_MObjAnimDesc* mat;
    HSD_PObjAnimDesc* pobj;

    if (dobj == NULL) {
        return;
    }
    cur = dobj;
    mat = mat_anims;
    pobj = pobj_anims;
    while (cur != NULL) {
        HSD_DObjAddAnim(cur, mat, pobj);
        cur = cur->next;
        mat = mat != NULL ? mat->next : NULL;
        pobj = pobj != NULL ? pobj->next : NULL;
    }
}

void HSD_DObjSetCurrent(HSD_DObj* dobj)
{
    current_dobj = dobj;
}

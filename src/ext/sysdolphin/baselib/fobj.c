#include <string.h>

#include <sysdolphin/baselib/fobj.h>

#include <sysdolphin/baselib/debug.h>
#include <sysdolphin/baselib/spline.h>

static HSD_ObjAllocData fobj_alloc_data;

HSD_ObjAllocData* HSD_FObjGetAllocData(void)
{
    return &fobj_alloc_data;
}

void HSD_FObjInit(void)
{
    HSD_ObjAllocInit(&fobj_alloc_data, sizeof(HSD_FObj), 4);
}

void HSD_FObjRemove(HSD_FObj* fobj)
{
    if (fobj != NULL) {
        HSD_FObjFree(fobj);
    }
}

void HSD_FObjRemoveAll(HSD_FObj* fobj)
{
    if (fobj == NULL) {
        return;
    }
    HSD_FObjRemoveAll(fobj->next);
    HSD_FObjRemove(fobj);
}

static inline HSD_FObjLoadState FObjGetState(HSD_FObj* fobj)
{
    if (fobj == NULL) {
        return FOBJ_LOAD_NONE;
    }
    return fobj->flags & FOBJ_STATE_MASK;
}

static inline HSD_FObjLoadState FObjSetState(HSD_FObj* fobj, HSD_FObjLoadState state)
{
    if (fobj != NULL) {
        fobj->flags = (state & FOBJ_STATE_MASK) | (fobj->flags & FOBJ_FLAG_MASK);
    }
    return state;
}

static inline void FObjReqAnim(HSD_FObj* fobj, f32 startframe)
{
    if (fobj == NULL) {
        return;
    }

    fobj->ad = fobj->ad_head;
    fobj->time = fobj->startframe + startframe;
    fobj->op = HSD_A_OP_NONE;
    fobj->op_intrp = HSD_A_OP_NONE;
    fobj->flags &= ~FOBJ_FLAG_KEY_PENDING;
    fobj->nb_pack = 0;
    fobj->fterm = 0;
    fobj->p0 = 0.0;
    fobj->p1 = 0.0;
    fobj->d0 = 0.0;
    fobj->d1 = 0.0;
    FObjSetState(fobj, FOBJ_LOAD_DATA0);
}

void HSD_FObjReqAnimAll(HSD_FObj* fobj, f32 startframe)
{
    HSD_FObj* fp;

    if (fobj == NULL) {
        return;
    }

    for (fp = fobj; fp != NULL; fp = fp->next) {
        FObjReqAnim(fp, startframe);
    }
}

static inline void FObjStopAnim(HSD_FObj* fobj, void* obj,
                                HSD_AObjUpdateCallback obj_update, f32 rate)
{
    if (fobj == NULL) {
        return;
    }
    if (fobj->op_intrp == HSD_A_OP_KEY) {
        HSD_FObjInterpretAnim(fobj, obj, obj_update, rate);
    }
    FObjSetState(fobj, FOBJ_LOAD_NONE);
}

void HSD_FObjStopAll(HSD_FObj* fobj, void* obj,
                     HSD_AObjUpdateCallback obj_update, f32 rate)
{
    for (; fobj != NULL; fobj = fobj->next) {
        FObjStopAnim(fobj, obj, obj_update, rate);
    }
}

static inline void FObjLaunchKeyData(HSD_FObj* fobj)
{
    if (fobj->flags & FOBJ_FLAG_KEY_PENDING) {
        fobj->op_intrp = fobj->op;
        fobj->flags &= ~FOBJ_FLAG_KEY_PENDING;
        fobj->flags |= FOBJ_FLAG_KEY_READY;
        fobj->p0 = fobj->p1;
    }
}

static inline f32 parseFloat(u8** pos, u8 frac)
{
    union {
        f32 f;
        u32 bits;
    } value;
    f32 numer;
    s32 denom;

    numer = 0.0f;
    if (frac == HSD_A_FRAC_FLOAT) {
        value.bits = *(*pos)++;
        value.bits |= *(*pos)++ << 8;
        value.bits |= *(*pos)++ << 16;
        value.bits |= *(*pos)++ << 24;
        return value.f;
    }

    denom = 1 << (frac & HSD_A_FRAC_SHIFT_MASK);
    switch (frac & HSD_A_FRAC_FORMAT_MASK) {
    case HSD_A_FRAC_S8:
        numer = (s8) (*pos)[0];
        *pos += 1;
        break;
    case HSD_A_FRAC_U8:
        numer = (*pos)[0];
        *pos += 1;
        break;
    case HSD_A_FRAC_S16:
        numer = ((s8) (*pos)[1] << 8) | (*pos)[0];
        *pos += 2;
        break;
    case HSD_A_FRAC_U16:
        numer = ((*pos)[1] << 8) | (*pos)[0];
        *pos += 2;
        break;
    default:
        return 0.0f;
    }
    return numer / denom;
}

static inline u8 parseOpCode(u8** curr_parse)
{
    return (**curr_parse) & HSD_A_OP_MASK;
}

static inline u32 parsePackInfo(u8** adp)
{
    u8 d;
    u32 nb_pack;
    s32 shift;

    d = *(*adp)++;
    nb_pack = ((d >> HSD_A_PACK_COUNT_SHIFT) & HSD_A_PACK_COUNT_MASK) + 1;
    shift = HSD_A_PACK_COUNT_CONT_SHIFT;
    if (!(d & HSD_A_MORE_BYTES)) {
        return nb_pack;
    }
    do {
        d = *(*adp)++;
        nb_pack += (d & HSD_A_BYTE_PAYLOAD) << shift;
        shift += HSD_A_BYTE_PAYLOAD_BITS;
    } while (d & HSD_A_MORE_BYTES);
    return nb_pack;
}

static inline s32 parseWait(u8** adp)
{
    u8 d;
    s32 wait;
    s32 shift;

    wait = 0;
    shift = 0;

    do {
        d = *(*adp)++;
        wait |= (d & HSD_A_BYTE_PAYLOAD) << shift;
        shift += HSD_A_BYTE_PAYLOAD_BITS;
    } while (d & HSD_A_MORE_BYTES);

    return wait;
}

static inline HSD_FObjLoadState FObjLoadWait(HSD_FObj* fobj)
{
    HSD_FObjLoadState st = FObjGetState(fobj);
    if ((u32) st != FOBJ_LOAD_WAIT) {
        HSD_AssertFail(__FILE__, 364, "st == FOBJ_LOAD_WAIT");
    }

    if (fobj->ad - fobj->ad_head >= fobj->length) {
        return FOBJ_LOAD_END;
    }
    fobj->fterm = parseWait(&fobj->ad);
    fobj->flags |= FOBJ_FLAG_SLOPE_DIRTY;
    return FObjSetState(fobj, FOBJ_LOAD_DATA);
}

static inline HSD_FObjLoadState FObjAnimCON(HSD_FObj* fobj)
{
    HSD_FObjLoadState st = FObjGetState(fobj);
    HSD_ASSERT(383, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);

    fobj->p0 = fobj->p1;
    fobj->p1 = parseFloat(&fobj->ad, fobj->frac_value);
    if (fobj->op_intrp != HSD_A_OP_SLP) {
        fobj->d0 = fobj->d1;
        fobj->d1 = 0.0f;
    }

    return FObjSetState(fobj, st == FOBJ_LOAD_DATA0 ? (u32) FOBJ_LOAD_WAIT
                                                   : FOBJ_LOAD_PLAY);
}

static inline HSD_FObjLoadState FObjAnimLinear(HSD_FObj* fobj)
{
    HSD_FObjLoadState st = FObjGetState(fobj);
    HSD_ASSERT(403, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);

    fobj->p0 = fobj->p1;
    fobj->p1 = parseFloat(&fobj->ad, fobj->frac_value);
    if (fobj->op_intrp != HSD_A_OP_SLP) {
        fobj->d0 = fobj->d1;
        fobj->d1 = 0.0f;
    }

    return FObjSetState(fobj, st == FOBJ_LOAD_DATA0 ? (u32) FOBJ_LOAD_WAIT
                                                   : FOBJ_LOAD_PLAY);
}

static inline HSD_FObjLoadState FObjAnimSPL0(HSD_FObj* fobj)
{
    HSD_FObjLoadState st = FObjGetState(fobj);
    HSD_ASSERT(423, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);

    fobj->p0 = fobj->p1;
    fobj->d0 = fobj->d1;
    fobj->p1 = parseFloat(&fobj->ad, fobj->frac_value);
    fobj->d1 = 0.0f;

    return FObjSetState(fobj, st == FOBJ_LOAD_DATA0 ? (u32) FOBJ_LOAD_WAIT
                                                   : FOBJ_LOAD_PLAY);
}

static inline HSD_FObjLoadState FObjAnimSPL(HSD_FObj* fobj)
{
    HSD_FObjLoadState st = FObjGetState(fobj);
    HSD_ASSERT(441, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);

    fobj->p0 = fobj->p1;
    fobj->p1 = parseFloat(&fobj->ad, fobj->frac_value);
    fobj->d0 = fobj->d1;
    fobj->d1 = parseFloat(&fobj->ad, fobj->frac_slope);

    return FObjSetState(fobj, st == FOBJ_LOAD_DATA0 ? (u32) FOBJ_LOAD_WAIT
                                                   : FOBJ_LOAD_PLAY);
}

static inline HSD_FObjLoadState FObjAnimSLP(HSD_FObj* fobj)
{
    HSD_FObjLoadState st = FObjGetState(fobj);
    HSD_ASSERT(460, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);

    fobj->d0 = fobj->d1;
    fobj->d1 = parseFloat(&fobj->ad, fobj->frac_slope);

    return FObjGetState(fobj);
}

static inline HSD_FObjLoadState FObjAnimKey(HSD_FObj* fobj)
{
    HSD_FObjLoadState st = FObjGetState(fobj);
    HSD_ASSERT(489, st == FOBJ_LOAD_DATA0 || st == FOBJ_LOAD_DATA);

    FObjLaunchKeyData(fobj);
    fobj->p1 = parseFloat(&fobj->ad, fobj->frac_value);
    fobj->flags |= FOBJ_FLAG_KEY_PENDING;

    return FObjSetState(fobj, st == FOBJ_LOAD_DATA0 ? (u32) FOBJ_LOAD_WAIT
                                                   : FOBJ_LOAD_PLAY);
}

HSD_FObjLoadState HSD_FObjLoadNextData(HSD_FObj* fobj)
{
    if (fobj->ad - fobj->ad_head >= fobj->length) {
        return FOBJ_LOAD_END;
    }
    fobj->op_intrp = fobj->op;
    if (fobj->nb_pack == 0) {
        fobj->op = parseOpCode(&fobj->ad);
        fobj->nb_pack = parsePackInfo(&fobj->ad);
    }

    fobj->nb_pack -= 1;

    switch (fobj->op) {
    case HSD_A_OP_CON:
        return FObjAnimCON(fobj);
    case HSD_A_OP_LIN:
        return FObjAnimLinear(fobj);
    case HSD_A_OP_SPL0:
        return FObjAnimSPL0(fobj);
    case HSD_A_OP_SPL:
        return FObjAnimSPL(fobj);
    case HSD_A_OP_SLP:
        return FObjAnimSLP(fobj);
    case HSD_A_OP_KEY:
        return FObjAnimKey(fobj);
    default:
        return FOBJ_LOAD_NONE;
    }
}

static inline void FObjUpdateAnim(HSD_FObj* fobj, void* obj,
                                  HSD_AObjUpdateCallback obj_update)
{
    HSD_ObjData fobjdata;

    if (obj_update == NULL) {
        return;
    }
    switch (fobj->op_intrp) {
    case HSD_A_OP_KEY:
        if (fobj->flags & FOBJ_FLAG_KEY_READY) {
            fobjdata.fv = fobj->p0;
            fobj->flags &= ~FOBJ_FLAG_KEY_READY;
        } else {
            return;
        }
        break;
    case HSD_A_OP_CON:
        fobjdata.fv = fobj->time >= fobj->fterm ? fobj->p1 : fobj->p0;
        break;
    case HSD_A_OP_LIN:
        if (fobj->flags & FOBJ_FLAG_SLOPE_DIRTY) {
            fobj->flags &= ~FOBJ_FLAG_SLOPE_DIRTY;
            if (fobj->fterm != 0) {
                fobj->d0 = (fobj->p1 - fobj->p0) / fobj->fterm;
            } else {
                fobj->d0 = 0.0f;
                fobj->p0 = fobj->p1;
            }
        }
        fobjdata.fv = fobj->d0 * fobj->time + fobj->p0;
        break;
    case HSD_A_OP_SPL0:
    case HSD_A_OP_SPL:
    case HSD_A_OP_SLP:
        if (fobj->fterm != 0) {
            fobjdata.fv = HSD_SplineEval(1.0 / fobj->fterm, fobj->time,
                                         fobj->p0, fobj->p1, fobj->d0,
                                         fobj->d1);
        } else {
            fobjdata.fv = fobj->p1;
        }
        break;
    default:
        break;
    }
    obj_update(obj, fobj->obj_type, &fobjdata.fv);
}

void HSD_FObjInterpretAnim(HSD_FObj* fobj, void* obj,
                           HSD_AObjUpdateCallback obj_update, f32 rate)
{
    f32 fterm;
    HSD_FObjLoadState state;

    fterm = 0.0f;
    state = fobj != NULL ? FObjGetState(fobj) : FOBJ_LOAD_NONE;

    if ((u32) state == FOBJ_LOAD_NONE) {
        return;
    }

    fobj->time += rate;

    if (fobj->time < 0.0) {
        return;
    }

    while (1) {
        switch (state) {
        case FOBJ_LOAD_END:
            fobj->time += fterm;
            FObjLaunchKeyData(fobj);
            FObjUpdateAnim(fobj, obj, obj_update);
            return;
        case FOBJ_LOAD_DATA0:
        case FOBJ_LOAD_DATA:
            state = HSD_FObjLoadNextData(fobj);
            break;
        case FOBJ_LOAD_WAIT:
            if (fobj->flags & FOBJ_FLAG_KEY_READY) {
                FObjUpdateAnim(fobj, obj, obj_update);
            }
            state = FObjLoadWait(fobj);
            break;
        case FOBJ_LOAD_PLAY:
            if (fobj->fterm <= fobj->time) {
                state = FOBJ_LOAD_WAIT;
                fterm = fobj->fterm;
                fobj->time -= fobj->fterm;
                FObjSetState(fobj, state);
                break;
            }
            FObjUpdateAnim(fobj, obj, obj_update);
            state = FOBJ_LOAD_RECHECK;
            FObjSetState(fobj, state);
            return;
        case FOBJ_LOAD_RECHECK:
            state = FOBJ_LOAD_PLAY;
            FObjSetState(fobj, state);
            break;
        case FOBJ_LOAD_NONE:
            return;
        }
    }
}

void HSD_FObjInterpretAnimAll(HSD_FObj* fobj, void* obj,
                              HSD_AObjUpdateCallback obj_update, f32 rate)
{
    HSD_FObj* fp;

    for (fp = fobj; fp != NULL; fp = fp->next) {
        HSD_FObjInterpretAnim(fp, obj, obj_update, rate);
    }
}

void HSD_FObjDebug(HSD_ObjAllocData* alloc, const char* label)
{
}

HSD_FObj* HSD_FObjLoadDesc(HSD_FObjDesc* desc)
{
    if (desc != NULL) {
        HSD_FObj* fobj = HSD_FObjAlloc();
        fobj->next = HSD_FObjLoadDesc(desc->next);
        fobj->startframe = desc->startframe;
        fobj->obj_type = desc->type;
        fobj->frac_value = desc->frac_value;
        fobj->frac_slope = desc->frac_slope;
        fobj->ad_head = desc->ad;
        fobj->length = desc->length;
        fobj->flags = 0;
        return fobj;
    }
    return NULL;
}

HSD_FObj* HSD_FObjAlloc(void)
{
    void* new = HSD_ObjAlloc(&fobj_alloc_data);
    HSD_FObjDebug(&fobj_alloc_data, "fobj: alloc");
    HSD_ASSERT(755, new);
    memset(new, 0, sizeof(HSD_FObj));
    return new;
}

void HSD_FObjFree(HSD_FObj* fobj)
{
    HSD_ObjFree(&fobj_alloc_data, fobj);
    HSD_FObjDebug(&fobj_alloc_data, "fobj: free");
}

#ifndef SYSDOLPHIN_BASELIB_FOBJ_H
#define SYSDOLPHIN_BASELIB_FOBJ_H

#include <dolphin/types.h>
#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/objalloc.h>

typedef enum _HSD_AnimOpcode {
    HSD_A_OP_NONE,
    HSD_A_OP_CON,
    HSD_A_OP_LIN,
    HSD_A_OP_SPL0,
    HSD_A_OP_SPL,
    HSD_A_OP_SLP,
    HSD_A_OP_KEY
} HSD_AnimOpcode;

typedef enum _HSD_AnimFrac {
    HSD_A_FRAC_FLOAT = 0 << 5,
    HSD_A_FRAC_S16 = 1 << 5,
    HSD_A_FRAC_U16 = 2 << 5,
    HSD_A_FRAC_S8 = 3 << 5,
    HSD_A_FRAC_U8 = 4 << 5
} HSD_AnimFrac;

static const u32 HSD_A_OP_MASK = 0x0F;
static const u32 HSD_A_FRAC_SHIFT_MASK = 0x1F;
static const u32 HSD_A_FRAC_FORMAT_MASK = 0xE0;

static const u32 HSD_A_MORE_BYTES = 0x80;
static const u32 HSD_A_BYTE_PAYLOAD = 0x7F;
static const u32 HSD_A_PACK_COUNT_MASK = 0x07;
static const u32 HSD_A_PACK_COUNT_SHIFT = 4;
static const u32 HSD_A_PACK_COUNT_CONT_SHIFT = 3;
static const u32 HSD_A_BYTE_PAYLOAD_BITS = 7;

typedef enum _HSD_FObjFlags {
    FOBJ_FLAG_SLOPE_DIRTY = 0x20,
    FOBJ_FLAG_KEY_PENDING = 0x40,
    FOBJ_FLAG_KEY_READY = 0x80
} HSD_FObjFlags;

static const u32 FOBJ_FLAG_MASK = 0xF0;
static const u32 FOBJ_STATE_MASK = 0x0F;

typedef enum _HSD_FObjLoadState {
    FOBJ_LOAD_NONE,
    FOBJ_LOAD_DATA0,
    FOBJ_LOAD_DATA,
    FOBJ_LOAD_WAIT,
    FOBJ_LOAD_PLAY,
    FOBJ_LOAD_RECHECK,
    FOBJ_LOAD_END
} HSD_FObjLoadState;

typedef union _HSD_ObjData {
    f32 fv;
    s32 iv;
    struct {
        f32 x, y, z;
    } p;
} HSD_ObjData;

typedef struct _HSD_FObj {
    struct _HSD_FObj* next;
    u8* ad;
    u8* ad_head;
    u32 length;
    u8 flags;
    u8 op;
    u8 op_intrp;
    u8 obj_type;
    u8 frac_value;
    u8 frac_slope;
    u16 nb_pack;
    s16 startframe;
    u16 fterm;
    f32 time;
    f32 p0;
    f32 p1;
    f32 d0;
    f32 d1;
} HSD_FObj;

typedef struct _HSD_FObjDesc {
    struct _HSD_FObjDesc* next;
    u32 length;
    f32 startframe;
    u8 type;
    u8 frac_value;
    u8 frac_slope;
    u8 dummy;
    u8* ad;
} HSD_FObjDesc;

void HSD_FObjFree(HSD_FObj* fobj);
HSD_FObj* HSD_FObjAlloc(void);
HSD_FObj* HSD_FObjLoadDesc(HSD_FObjDesc* desc);
void HSD_FObjDebug(HSD_ObjAllocData* alloc, const char* label);
void HSD_FObjInterpretAnimAll(HSD_FObj* fobj, void* obj,
                              HSD_AObjUpdateCallback obj_update, f32 rate);
void HSD_FObjInterpretAnim(HSD_FObj* fobj, void* obj,
                           HSD_AObjUpdateCallback obj_update, f32 rate);
HSD_FObjLoadState HSD_FObjLoadNextData(HSD_FObj* fobj);
void HSD_FObjStopAll(HSD_FObj* fobj, void* obj,
                     HSD_AObjUpdateCallback obj_update, f32 rate);
void HSD_FObjReqAnimAll(HSD_FObj* fobj, f32 startframe);
void HSD_FObjRemoveAll(HSD_FObj* fobj);
void HSD_FObjRemove(HSD_FObj* fobj);
void HSD_FObjInit(void);
HSD_ObjAllocData* HSD_FObjGetAllocData(void);

#endif

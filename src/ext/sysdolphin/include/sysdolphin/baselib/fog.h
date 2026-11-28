#ifndef SYSDOLPHIN_BASELIB_FOG_H
#define SYSDOLPHIN_BASELIB_FOG_H

#include <dolphin/gx/GXEnum.h>
#include <dolphin/gx/GXStruct.h>
#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/class.h>
#include <sysdolphin/baselib/object.h>

typedef struct _HSD_FogAdj HSD_FogAdj;
typedef struct _HSD_FogAdjDesc HSD_FogAdjDesc;
typedef struct _HSD_Fog HSD_Fog;
typedef struct _HSD_FogDesc HSD_FogDesc;

typedef enum _HSD_FogTrack {
    FOG_TRACK_START = 1,
    FOG_TRACK_END = 2,
    FOG_TRACK_COLOR_R = 5,
    FOG_TRACK_COLOR_G = 6,
    FOG_TRACK_COLOR_B = 7,
    FOG_TRACK_COLOR_A = 8,
    FOGADJ_TRACK_CENTER = 20,
    FOGADJ_TRACK_WIDTH = 21
} HSD_FogTrack;

typedef enum _HSD_FogAdjFlags {
    FOGADJ_NONE = 0,
    FOGADJ_CENTER = 0x1,
    FOGADJ_WIDTH = 0x2,
    FOGADJ_PROJECTION = 0x4
} HSD_FogAdjFlags;

typedef struct _HSD_FogInfo {
    HSD_ClassInfo info;
    void (*anim)(HSD_Fog* fog, HSD_FogTrack track, f32* value);
} HSD_FogInfo;

struct _HSD_Fog {
    HSD_Obj parent;
    GXFogType type;
    HSD_FogAdj* fogadj;
    f32 start;
    f32 end;
    GXColor color;
    HSD_AObj* aobj;
};

struct _HSD_FogAdj {
    HSD_Obj parent;
    HSD_FogAdjFlags flags;
    s16 centerOffset;
    u16 width;
    f32 projection[4][4];
    u32 reserved;
};

struct _HSD_FogDesc {
    GXFogType type;
    HSD_FogAdjDesc* adj;
    f32 start;
    f32 end;
    GXColor color;
    HSD_AObjDesc* aobjdesc;
};

struct _HSD_FogAdjDesc {
    HSD_FogAdjFlags flags;
    u16 centerOffset;
    u16 width;
    f32 projection[4][4];
};

extern HSD_FogInfo hsdFog;
extern HSD_ClassInfo hsdFogAdj;

void HSD_FogInit(void);
void HSD_FogAdjInit(void);
void HSD_FogRelease(HSD_Fog* fog);
void HSD_FogSetParam(HSD_Fog* fog, HSD_FogTrack track, f32* value);
HSD_Fog* HSD_FogLoad(HSD_FogDesc* desc);
void HSD_FogSet(HSD_Fog* fog);

#endif

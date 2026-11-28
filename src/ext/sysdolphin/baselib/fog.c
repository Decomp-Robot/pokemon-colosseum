#include <dolphin/gx/GXStruct.h>
#include <dolphin/gx/GXPixel.h>
#include <dolphin/mtx.h>
#include <string.h>
#include <dolphin/types.h>

#include <sysdolphin/baselib/aobj.h>
#include <sysdolphin/baselib/class.h>
#include <sysdolphin/baselib/cobj.h>
#include <sysdolphin/baselib/debug.h>
#include <sysdolphin/baselib/fog.h>
#include <sysdolphin/baselib/object.h>

void HSD_FogAdjInit(void)
{
    HSD_ClassInfoInit(&hsdFogAdj, &hsd_ObjInfo, "sysdolphin_base_library", "hsd_fogadj",
                      sizeof(HSD_ClassInfo), sizeof(HSD_FogAdj));
}

void HSD_FogInit(void)
{
    HSD_ClassInfoInit(&hsdFog.info, &hsd_ObjInfo, "sysdolphin_base_library", "hsd_fog",
                      sizeof(HSD_FogInfo), sizeof(HSD_Fog));
    hsdFog.info.release = (void (*)(void*)) HSD_FogRelease;
    hsdFog.anim = HSD_FogSetParam;
}

HSD_FogInfo hsdFog = { HSD_FogInit };
HSD_ClassInfo hsdFogAdj = { HSD_FogAdjInit };

void HSD_FogRelease(HSD_Fog* fog)
{
    HSD_FogAdj* fogadj = fog->fogadj;

    if (fogadj != NULL) {
        if (HSD_ObjUnref(HSD_OBJ(fogadj)) && fogadj != NULL) {
            fogadj->parent.class_info->release(fogadj);
            fogadj->parent.class_info->destroy(fogadj);
        }
    }
    HSD_AObjRelease(fog->aobj);
    hsdFog.info.parent->release(fog);
}

void HSD_FogSetParam(HSD_Fog* fog, HSD_FogTrack track, f32* value)
{
    if (fog != NULL) {
        switch (track) {
        case FOG_TRACK_START:
            fog->start = *value;
            break;
        case FOG_TRACK_END:
            fog->end = *value;
            break;
        case FOG_TRACK_COLOR_R: {
            f32 r = *value;

            if (r <= 0.0f) {
                r = 0.0f;
            } else if (r >= 1.0f) {
                r = 1.0f;
            }
            fog->color.r = 255.0f * r;
            break;
        }
        case FOG_TRACK_COLOR_G: {
            f32 g = *value;

            if (g <= 0.0f) {
                g = 0.0f;
            } else if (g >= 1.0f) {
                g = 1.0f;
            }
            fog->color.g = 255.0f * g;
            break;
        }
        case FOG_TRACK_COLOR_B: {
            f32 b = *value;

            if (b <= 0.0f) {
                b = 0.0f;
            } else if (b >= 1.0f) {
                b = 1.0f;
            }
            fog->color.b = 255.0f * b;
            break;
        }
        case FOG_TRACK_COLOR_A: {
            f32 a = *value;

            if (a <= 0.0f) {
                a = 0.0f;
            } else if (a >= 1.0f) {
                a = 1.0f;
            }
            fog->color.a = 255.0f * a;
            break;
        }
        case FOGADJ_TRACK_CENTER: {
            s32 centerOffset = (s32) *value;
            HSD_FogAdj* adj = (fog != NULL) ? fog->fogadj : NULL;

            if (adj != NULL) {
                if (centerOffset <= -320) {
                    adj->centerOffset = -320;
                } else if (centerOffset >= 320) {
                    adj->centerOffset = 320;
                } else {
                    adj->centerOffset = centerOffset;
                }
            }
            break;
        }
        case FOGADJ_TRACK_WIDTH: {
            s32 width = (s32) *value;
            HSD_FogAdj* adj = (fog != NULL) ? fog->fogadj : NULL;

            if (adj != NULL) {
                if (width <= 0) {
                    adj->width = 0;
                } else if (width >= 640) {
                    adj->width = 640;
                } else {
                    adj->width = width;
                }
            }
            break;
        }
        }
    }
}

static void FogAdjInitFromDesc(HSD_FogAdj* adj, HSD_FogAdjDesc* adjdesc)
{
    HSD_ASSERT(265, adj);
    if (adj != NULL) {
        if (adjdesc != NULL) {
            adj->flags = adjdesc->flags;
            adj->width = adjdesc->width;
            adj->centerOffset = adjdesc->centerOffset;
            memcpy(adj->projection, adjdesc->projection, sizeof(adj->projection));
        } else {
            adj->flags = FOGADJ_NONE;
            adj->width = 0;
            adj->centerOffset = 0;
            memset(adj->projection, 0, sizeof(adj->projection));
        }
    }
}

static void FogInitFromDesc(HSD_Fog* fog, HSD_FogDesc* desc)
{
    HSD_ASSERT(174, fog);

    if (fog != NULL) {
        if (desc != NULL) {
            fog->type = desc->type;
            fog->start = desc->start;
            fog->end = desc->end;
            fog->color = desc->color;
        } else {
            f32 viewport[6];

            GXGetViewportv(viewport);
            fog->type = GX_FOG_LIN;
            fog->start = viewport[4];
            fog->end = viewport[5];
            fog->color.r = 255;
            fog->color.g = 255;
            fog->color.b = 255;
            fog->color.a = 255;
        }
    }
}

HSD_Fog* HSD_FogLoad(HSD_FogDesc* desc)
{
    HSD_Fog* result;
    HSD_FogAdjDesc* adjdesc;
    HSD_FogAdj* adj;

    {
        HSD_Fog* fog = hsdNew(&hsdFog.info);
        HSD_ASSERT(161, fog);
        result = fog;
    }
    FogInitFromDesc(result, desc);

    if (desc->adj != NULL) {
        adjdesc = desc->adj;

        adj = hsdNew(&hsdFogAdj);
        HSD_ASSERT(249, adj);
        FogAdjInitFromDesc(adj, adjdesc);
        result->fogadj = adj;
    }
    return result;
}

void HSD_FogSet(HSD_Fog* fog)
{
    GXFogAdjTable adj_table;
    HSD_CObj* cobj;
    HSD_FogAdj* fogadj;
    HSD_FogAdjFlags flags;
    f32 viewport[6];
    f32 projection[7];
    f32 matrix[4][4];
    f32 (*source)[4];
    s32 center;
    s32 width;

    if (fog == NULL) {
        GXColor color = { 0, 0, 0, 0 };

        GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, color);
    } else {
        cobj = HSD_CObjGetCurrent();

        if (cobj == NULL) {
            HSD_ERR(88, "You must specify CObj first.\n");
        }
        GXSetFog(fog->type, fog->start, fog->end, HSD_CObjGetNear(cobj), HSD_CObjGetFar(cobj),
                 fog->color);

        fogadj = (fog != NULL) ? fog->fogadj : NULL;
        flags = (fogadj != NULL) ? fogadj->flags : FOGADJ_NONE;

        if (fogadj != NULL &&
            (flags & (FOGADJ_CENTER | FOGADJ_WIDTH | FOGADJ_PROJECTION)) != 0) {
            GXGetViewportv(viewport);
            if ((flags & FOGADJ_CENTER) != 0) {
                center = (s32)(viewport[0] + viewport[2] *
                               (f32)(((fogadj != NULL) ? fogadj->centerOffset : -1) + 320) / 640.0f);
            } else {
                center = (s32)(viewport[2] / 2.0f + viewport[0]);
            }
            if ((flags & FOGADJ_WIDTH) != 0) {
                width = (fogadj != NULL) ? fogadj->width : -1;
            } else {
                width = (s32) viewport[2];
            }
            if ((flags & FOGADJ_PROJECTION) != 0) {
                source = (fogadj != NULL) ? fogadj->projection : NULL;
            } else {
                source = matrix;
                memset(matrix, 0, sizeof(matrix));
                GXGetProjectionv(projection);
                switch ((s32) projection[0]) {
                case GX_PERSPECTIVE:
                    matrix[0][0] = projection[1];
                    matrix[0][2] = projection[2];
                    matrix[1][1] = projection[3];
                    matrix[1][2] = projection[4];
                    matrix[2][2] = projection[5];
                    matrix[2][3] = projection[6];
                    matrix[3][2] = -1.0f;
                    break;
                default:
                    matrix[0][0] = projection[1];
                    matrix[0][3] = projection[2];
                    matrix[1][1] = projection[3];
                    matrix[1][3] = projection[4];
                    matrix[2][2] = projection[5];
                    matrix[2][3] = projection[6];
                    matrix[3][3] = 1.0f;
                    break;
                }
            }
            GXInitFogAdjTable(&adj_table, width, source);
            GXSetFogRangeAdj(GX_TRUE, center, &adj_table);
        } else {
            GXSetFogRangeAdj(GX_FALSE, 0, NULL);
        }
    }
}

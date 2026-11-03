#include <dolphin/gx.h>
#include <dolphin/os.h>
#include <macros.h>

#include "__gx.h"

void GXSetTevColorIn(GXTevStageID stage, GXTevColorArg a, GXTevColorArg b, GXTevColorArg c, GXTevColorArg d)
{
    u32 tev;

    CHECK_GXBEGIN(0xE5, "GXSetTevColorIn");
    ASSERTMSGLINE(0xE6, stage < 16, "GXSetTevColor: Invalid Tev Stage Index");
    ASSERTMSGLINE(0xE7, a <= 15, "GXSetTev*In: A/B/C/D argument out of range");
    ASSERTMSGLINE(0xE8, b <= 15, "GXSetTev*In: A/B/C/D argument out of range");
    ASSERTMSGLINE(0xE9, c <= 15, "GXSetTev*In: A/B/C/D argument out of range");
    ASSERTMSGLINE(0xEA, d <= 15, "GXSetTev*In: A/B/C/D argument out of range");

    tev = gx->tevc[stage];
    tev = __rlwimi(tev, a, 12, 16, 19);
    tev = __rlwimi(tev, b, 8, 20, 23);
    tev = __rlwimi(tev, c, 4, 24, 27);
    tev = __rlwimi(tev, d, 0, 28, 31);

    GX_WRITE_RAS_REG(tev);
    gx->tevc[stage] = tev;
    gx->bpSent = 0;
}

void GXSetTevAlphaIn(GXTevStageID stage, GXTevAlphaArg a, GXTevAlphaArg b, GXTevAlphaArg c, GXTevAlphaArg d)
{
    u32 tev;

    CHECK_GXBEGIN(0x108, "GXSetTevAlphaIn");
    ASSERTMSGLINE(0x109, stage < 16, "GXSetTevAlpha: Invalid Tev Stage Index");
    ASSERTMSGLINE(0x10A, a <= 7, "GXSetTev*In: A/B/C/D argument out of range");
    ASSERTMSGLINE(0x10B, b <= 7, "GXSetTev*In: A/B/C/D argument out of range");
    ASSERTMSGLINE(0x10C, c <= 7, "GXSetTev*In: A/B/C/D argument out of range");
    ASSERTMSGLINE(0x10D, d <= 7, "GXSetTev*In: A/B/C/D argument out of range");

    tev = gx->teva[stage];
    tev = __rlwimi(tev, a, 13, 16, 18);
    tev = __rlwimi(tev, b, 10, 19, 21);
    tev = __rlwimi(tev, c, 7, 22, 24);
    tev = __rlwimi(tev, d, 4, 25, 27);

    GX_WRITE_RAS_REG(tev);
    gx->teva[stage] = tev;
    gx->bpSent = 0;
}

void GXSetTevColorOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale, GXBool clamp, GXTevRegID out_reg)
{
    u32 tev;

    CHECK_GXBEGIN(0x12E, "GXSetTevColorOp");
    ASSERTMSGLINE(0x12F, stage < 16, "GXSetTevColor: Invalid Tev Stage Index");

    tev = gx->tevc[stage];
    tev = __rlwimi(tev, op & 1, 18, 13, 13);
    if (op <= 1) {
        tev = __rlwimi(tev, scale, 20, 10, 11);
        tev = __rlwimi(tev, bias, 16, 14, 15);
    } else {
        tev = __rlwimi(tev, op, 19, 10, 11);
        tev = __rlwimi(tev, 3, 16, 14, 15);
    }
    tev = __rlwimi(tev, clamp, 19, 12, 12);
    tev = __rlwimi(tev, out_reg, 22, 8, 9);

    GX_WRITE_RAS_REG(tev);
    gx->tevc[stage] = tev;
    gx->bpSent = 0;
}

void GXSetTevAlphaOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale, GXBool clamp, GXTevRegID out_reg)
{
    u32 tev;

    CHECK_GXBEGIN(0x15A, "GXSetTevAlphaOp");
    ASSERTMSGLINE(0x15B, stage < 16, "GXSetTevAlpha: Invalid Tev Stage Index");

    tev = gx->teva[stage];
    tev = __rlwimi(tev, op & 1, 18, 13, 13);
    if (op <= 1) {
        tev = __rlwimi(tev, scale, 20, 10, 11);
        tev = __rlwimi(tev, bias, 16, 14, 15);
    } else {
        tev = __rlwimi(tev, op, 19, 10, 11);
        tev = __rlwimi(tev, 3, 16, 14, 15);
    }
    tev = __rlwimi(tev, clamp, 19, 12, 12);
    tev = __rlwimi(tev, out_reg, 22, 8, 9);

    GX_WRITE_RAS_REG(tev);
    gx->teva[stage] = tev;
    gx->bpSent = 0;
}

void GXSetTevColor(GXTevRegID id, GXColor color)
{
    u32 regRA;
    u32 regBG;

    CHECK_GXBEGIN(0x182, "GXSetTevColor");

    regRA = 0;
    SET_REG_FIELD(0x185, regRA, 11,  0, color.r);
    SET_REG_FIELD(0x186, regRA, 11, 12, color.a);
    SET_REG_FIELD(0x187, regRA,  8, 24, 224 + id * 2);

    regBG = 0;
    SET_REG_FIELD(0x18A, regBG, 11,  0, color.b);
    SET_REG_FIELD(0x18B, regBG, 11, 12, color.g);
    SET_REG_FIELD(0x18C, regBG,  8, 24, 225 + id * 2);

    GX_WRITE_RAS_REG(regRA);
    GX_WRITE_RAS_REG(regBG);
    GX_WRITE_RAS_REG(regBG);
    GX_WRITE_RAS_REG(regBG);
    gx->bpSent = 0;
}

void GXSetTevColorS10(GXTevRegID id, GXColorS10 color)
{
    u32 regRA;
    u32 regBG;

    ASSERTMSGLINE(0x1A7, color.r >= -1024 && color.r < 1024, "GXSetTevColorS10: Color not in range -1024 to +1023");
    ASSERTMSGLINE(0x1A8, color.g >= -1024 && color.g < 1024, "GXSetTevColorS10: Color not in range -1024 to +1023");
    ASSERTMSGLINE(0x1A9, color.b >= -1024 && color.b < 1024, "GXSetTevColorS10: Color not in range -1024 to +1023");
    ASSERTMSGLINE(0x1AA, color.a >= -1024 && color.a < 1024, "GXSetTevColorS10: Color not in range -1024 to +1023");

    CHECK_GXBEGIN(0x1AC, "GXSetTevColorS10");

    regRA = 0;
    SET_REG_FIELD(0x1AF, regRA, 11,  0, color.r & 0x7FF);
    SET_REG_FIELD(0x1B0, regRA, 11, 12, color.a & 0x7FF);
    SET_REG_FIELD(0x1B1, regRA,  8, 24, 224 + id * 2);

    regBG = 0;
    SET_REG_FIELD(0x1B4, regBG, 11,  0, color.b & 0x7FF);
    SET_REG_FIELD(0x1B5, regBG, 11, 12, color.g & 0x7FF);
    SET_REG_FIELD(0x1B6, regBG,  8, 24, 225 + id * 2);

    GX_WRITE_RAS_REG(regRA);
    GX_WRITE_RAS_REG(regBG);
    GX_WRITE_RAS_REG(regBG);
    GX_WRITE_RAS_REG(regBG);
    gx->bpSent = 0;
}

void GXSetTevKColor(GXTevKColorID id, GXColor color)
{
    u32 regRA;
    u32 regBG;

    CHECK_GXBEGIN(0x1DD, "GXSetTevKColor");

    regRA = 0;
    SET_REG_FIELD(0x1E0, regRA, 8,  0, color.r);
    SET_REG_FIELD(0x1E1, regRA, 8, 12, color.a);
    SET_REG_FIELD(0x1E2, regRA, 4, 20, 8);
    SET_REG_FIELD(0x1E3, regRA, 8, 24, 224 + id * 2);

    regBG = 0;
    SET_REG_FIELD(0x1E6, regBG, 8,  0, color.b);
    SET_REG_FIELD(0x1E7, regBG, 8, 12, color.g);
    SET_REG_FIELD(0x1E8, regBG, 4, 20, 8);
    SET_REG_FIELD(0x1E9, regBG, 8, 24, 225 + id * 2);

    GX_WRITE_RAS_REG(regRA);
    GX_WRITE_RAS_REG(regBG);
    gx->bpSent = 0;
}

void GXSetTevKColorSel(GXTevStageID stage, GXTevKColorSel sel)
{
    u32* Kreg;

    CHECK_GXBEGIN(0x204, "GXSetTevKColorSel");
    ASSERTMSGLINE(0x205, stage < 16, "GXSetTevKColor*: Invalid Tev Stage Index");

    Kreg = &gx->tevKsel[stage >> 1];
    if (stage & 1) {
        SET_REG_FIELD(0x20A, *Kreg, 5, 14, sel);
    } else {
        SET_REG_FIELD(0x20C, *Kreg, 5, 4, sel);
    }

    GX_WRITE_RAS_REG(*Kreg);
    gx->bpSent = 0;
}

void GXSetTevKAlphaSel(GXTevStageID stage, GXTevKAlphaSel sel)
{
    u32* Kreg;

    CHECK_GXBEGIN(0x225, "GXSetTevKAlphaSel");
    ASSERTMSGLINE(0x226, stage < 16, "GXSetTevKColor*: Invalid Tev Stage Index");

    Kreg = &gx->tevKsel[stage >> 1];
    if (stage & 1) {
        SET_REG_FIELD(0x22B, *Kreg, 5, 19, sel);
    } else {
        SET_REG_FIELD(0x22D, *Kreg, 5, 9, sel);
    }

    GX_WRITE_RAS_REG(*Kreg);
    gx->bpSent = 0;
}

void GXSetTevSwapMode(GXTevStageID stage, GXTevSwapSel ras_sel, GXTevSwapSel tex_sel)
{
    u32* pTevReg;

    CHECK_GXBEGIN(0x24A, "GXSetTevSwapMode");
    ASSERTMSGLINE(0x24B, stage < 16, "GXSetTevSwapMode: Invalid Tev Stage Index");

    pTevReg = &gx->teva[stage];
    SET_REG_FIELD(0x24E, *pTevReg, 2, 0, ras_sel);
    SET_REG_FIELD(0x24F, *pTevReg, 2, 2, tex_sel);

    GX_WRITE_RAS_REG(*pTevReg);
    gx->bpSent = 0;
}

void GXSetTevSwapModeTable(GXTevSwapSel table, GXTevColorChan red, GXTevColorChan green, GXTevColorChan blue, GXTevColorChan alpha)
{
    u32* Kreg;
#if !DEBUG
    // not a real variable, but needed to match release
    int index = table * 2;
#endif

    CHECK_GXBEGIN(0x26E, "GXSetTevSwapModeTable");
    ASSERTMSGLINE(0x26F, table < 4, "GXSetTevSwapModeTable: Invalid Swap Selection Index");

#if DEBUG
    Kreg = &gx->tevKsel[table * 2];
#else
    Kreg = &gx->tevKsel[index];
#endif
    SET_REG_FIELD(0x272, *Kreg, 2, 0, red);
    SET_REG_FIELD(0x273, *Kreg, 2, 2, green);

    GX_WRITE_RAS_REG(*Kreg);

    Kreg = &gx->tevKsel[table * 2 + 1];
    SET_REG_FIELD(0x277, *Kreg, 2, 0, blue);
    SET_REG_FIELD(0x278, *Kreg, 2, 2, alpha);

    GX_WRITE_RAS_REG(*Kreg);
    gx->bpSent = 0;
}

void GXSetAlphaCompare(GXCompare comp0, u8 ref0, GXAlphaOp op, GXCompare comp1, u8 ref1)
{
    u32 reg = 0;

    CHECK_GXBEGIN(0x2B2, "GXSetAlphaCompare");

    SET_REG_FIELD(0x2B4, reg, 8, 0, ref0);
    SET_REG_FIELD(0x2B5, reg, 8, 24, 0xF3);
    SET_REG_FIELD(0x2B6, reg, 8, 8, ref1);
    SET_REG_FIELD(0x2B7, reg, 3, 16, comp0);
    SET_REG_FIELD(0x2B8, reg, 3, 19, comp1);
    SET_REG_FIELD(0x2B9, reg, 2, 22, op);

    GX_WRITE_RAS_REG(reg);
    gx->bpSent = 0;
}

void GXSetZTexture(GXZTexOp op, GXTexFmt fmt, u32 bias)
{
    u32 zenv0;
    u32 zenv1;
    u32 type;

    CHECK_GXBEGIN(0x2D1, "GXSetZTexture");

    zenv0 = 0;
    SET_REG_FIELD(0x2D4, zenv0, 24, 0, bias);
    SET_REG_FIELD(0x2D5, zenv0, 8, 24, 0xF4);

    zenv1 = 0;
    switch (fmt)
    {
    case GX_TF_Z8:
        type = 0;
        break;
    case GX_TF_Z16:
        type = 1;
        break;
    case GX_TF_Z24X8:
        type = 2;
        break;
    default:
        ASSERTMSGLINE(0x2DD, 0, "GXSetZTexture: Invalid z-texture format");
        type = 2;
        break;
    }
    SET_REG_FIELD(0x2E0, zenv1, 2, 0, type);
    SET_REG_FIELD(0x2E1, zenv1, 2, 2, op);
    SET_REG_FIELD(0x2E2, zenv1, 8, 24, 0xF5);

    GX_WRITE_RAS_REG(zenv0);
    GX_WRITE_RAS_REG(zenv1);
    gx->bpSent = 0;
}

void GXSetTevOrder(GXTevStageID stage, GXTexCoordID coord, GXTexMapID map, GXChannelID color)
{
    u32* ptref;
    u32 tmap;
    u32 tcoord;
    static int c2r[] = { 0, 1, 0, 1, 0, 1, 7, 5, 6 };

    CHECK_GXBEGIN(0x307, "GXSetTevOrder");
    ASSERTMSGLINE(0x308, stage < 16, "GXSetTevColor: Invalid Tev Stage Index");

    ptref = &gx->tref[stage / 2];
    gx->texmapId[stage] = map;

    tmap = map & ~0x100;
    tmap = (tmap >= GX_MAX_TEXMAP) ? GX_TEXMAP0 : tmap;
    if (coord >= GX_MAX_TEXCOORD) {
        u32* ptexCoordId = &gx->unk_4E0;
        *ptexCoordId = gx->unk_4E0 & ~(1 << stage);
        tcoord = GX_TEXCOORD0;
    } else {
        u32* ptexCoordId = &gx->unk_4E0;
        *ptexCoordId = gx->unk_4E0 | (1 << stage);
        tcoord = coord;
    }

    if (stage & 1) {
        SET_REG_FIELD(0x314, *ptref, 3, 12, tmap);
        SET_REG_FIELD(0x315, *ptref, 3, 15, tcoord);
        SET_REG_FIELD(0x317, *ptref, 3, 19, (color == GX_COLOR_NULL) ? 7 : c2r[color]);
        SET_REG_FIELD(0x319, *ptref, 1, 18, (map != GX_TEXMAP_NULL && !(map & 0x100)));
    } else {
        SET_REG_FIELD(0x31C, *ptref, 3, 0, tmap);
        SET_REG_FIELD(0x31D, *ptref, 3, 3, tcoord);
        SET_REG_FIELD(0x31F, *ptref, 3, 7, (color == GX_COLOR_NULL) ? 7 : c2r[color]);
        SET_REG_FIELD(0x321, *ptref, 1, 6, (map != GX_TEXMAP_NULL && !(map & 0x100)));
    }

    GX_WRITE_RAS_REG(*ptref);
    gx->bpSent = 0;
    gx->dirtyState |= 1;
}

void GXSetNumTevStages(u8 nStages)
{
    CHECK_GXBEGIN(0x331, "GXSetNumTevStages");

    ASSERTMSGLINE(0x333, nStages != 0 && nStages <= 16, "GXSetNumTevStages: Exceed max number of tex stages");
    SET_REG_FIELD(0x334, gx->genMode, 4, 10, nStages - 1);
    gx->dirtyState |= 4;
}

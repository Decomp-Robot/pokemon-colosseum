#include <math.h>
#include <dolphin/gx.h>
#include <dolphin/os.h>
#include <macros.h>

#include "__gx.h"

// Bit fields of the raster registers are merged into the target word in place.
// The retail binary emits a lone `rlwimi` for every field merge (for example
// `rlwimi r6, r3, 3, 28, 28` in GXSetColorUpdate), i.e. the destination word is
// read from the register that already holds the previous fields.  Spelling the
// merge out as `(reg & ~mask) | (val << shift)` makes the vendored compilers
// keep a redundant `rlwinm` destination clear in front of every merge, so the
// merges are written with the rotate-and-mask intrinsic instead.
#define MERGE_FIELD(reg, val, size, shift) \
    ((reg) = __rlwimi((u32)(reg), (u32)(val), (shift), 32 - ((shift) + (size)), 31 - (shift)))

void GXSetFogRangeAdj(GXBool enable, u16 center, GXFogAdjTable *table)
{
    u32 i;
    u32 range_adj;
    u32 range_c;

    CHECK_GXBEGIN(0x106, "GXSetFogRangeAdj");

    if (enable) {
        ASSERTMSGLINE(0x109, table != NULL, "GXSetFogRangeAdj: table pointer is null");
        for (i = 0; i < 10; i += 2) {
            range_adj = 0;
            SET_REG_FIELD(0x10D, range_adj, 12, 0, table->r[i]);
            SET_REG_FIELD(0x10E, range_adj, 12, 12, table->r[i + 1]);
            SET_REG_FIELD(0x10F, range_adj, 8, 24, (i >> 1) + 0xE9);
            GX_WRITE_RAS_REG(range_adj);
        }
    }
    range_c = 0;
    SET_REG_FIELD(0x115, range_c, 10, 0, center + 342);
    SET_REG_FIELD(0x116, range_c, 1, 10, enable);
    SET_REG_FIELD(0x117, range_c, 8, 24, 0xE8);
    GX_WRITE_RAS_REG(range_c);
    gx->bpSent = 0;
}

void GXSetBlendMode(GXBlendMode type, GXBlendFactor src_factor, GXBlendFactor dst_factor, GXLogicOp op)
{
    u32 reg;

    CHECK_GXBEGIN(0x12F, "GXSetBlendMode");

    reg = gx->cmode0;
    MERGE_FIELD(reg, (type == GX_BM_SUBTRACT), 1, 11);
    MERGE_FIELD(reg, type & 1, 1, 0);
    MERGE_FIELD(reg, (type == GX_BM_LOGIC), 1, 1);
    MERGE_FIELD(reg, op, 4, 12);
    MERGE_FIELD(reg, src_factor, 3, 8);
    MERGE_FIELD(reg, dst_factor, 3, 5);
    GX_WRITE_RAS_REG(reg);
    gx->cmode0 = reg;
    gx->bpSent = 0;
}

void GXSetColorUpdate(GXBool update_enable)
{
    u32 reg;

    CHECK_GXBEGIN(0x14F, "GXSetColorUpdate");
    reg = gx->cmode0;
    MERGE_FIELD(reg, update_enable, 1, 3);
    GX_WRITE_RAS_REG(reg);
    gx->cmode0 = reg;
    gx->bpSent = 0;
}

void GXSetAlphaUpdate(GXBool update_enable)
{
    u32 reg;

    CHECK_GXBEGIN(0x15A, "GXSetAlphaUpdate");
    reg = gx->cmode0;
    MERGE_FIELD(reg, update_enable, 1, 4);
    GX_WRITE_RAS_REG(reg);
    gx->cmode0 = reg;
    gx->bpSent = 0;
}

void GXSetZMode(GXBool compare_enable, GXCompare func, GXBool update_enable)
{
    u32 reg;

    CHECK_GXBEGIN(0x170, "GXSetZMode");
    reg = gx->zmode;
    MERGE_FIELD(reg, compare_enable, 1, 0);
    MERGE_FIELD(reg, func, 3, 1);
    MERGE_FIELD(reg, update_enable, 1, 4);
    GX_WRITE_RAS_REG(reg);
    gx->zmode = reg;
    gx->bpSent = 0;
}

void GXSetZCompLoc(GXBool before_tex)
{
    CHECK_GXBEGIN(0x17C, "GXSetZCompLoc");
    SET_REG_FIELD(0x17D, gx->peCtrl, 1, 6, before_tex);
    GX_WRITE_RAS_REG(gx->peCtrl);
    gx->bpSent = 0;
}

void GXSetPixelFmt(GXPixelFmt pix_fmt, GXZFmt16 z_fmt)
{
    u32 oldPeCtrl;
    u8 aa;
    static u32 p2f[8] = { 0, 1, 2, 3, 4, 4, 4, 5 };

    CHECK_GXBEGIN(0x1A1, "GXSetPixelFmt");
    oldPeCtrl = gx->peCtrl;
    ASSERTMSGLINE(0x1A5, pix_fmt >= 0 && pix_fmt <= 7, "Invalid Pixel format");
    SET_REG_FIELD(0x1A7, gx->peCtrl, 3, 0, p2f[pix_fmt]);
    SET_REG_FIELD(0x1A8, gx->peCtrl, 3, 3, z_fmt);
    if (oldPeCtrl != gx->peCtrl) {
        GX_WRITE_RAS_REG(gx->peCtrl);
        if (pix_fmt == GX_PF_RGB565_Z16)
            aa = 1;
        else
            aa = 0;
        SET_REG_FIELD(0x1B1, gx->genMode, 1, 9, aa);
        gx->dirtyState |= 4;
    }
    if (p2f[pix_fmt] == 4) {
        SET_REG_FIELD(0x1B8, gx->cmode1, 2, 9, (pix_fmt - 4) & 0x3);
        SET_REG_FIELD(0x1B9, gx->cmode1, 8, 24, 0x42);
        GX_WRITE_RAS_REG(gx->cmode1);
    }
    gx->bpSent = 0;
}

void GXSetDither(GXBool dither)
{
    u32 reg;

    CHECK_GXBEGIN(0x1D6, "GXSetDither");
    reg = gx->cmode0;
    MERGE_FIELD(reg, dither, 1, 2);
    GX_WRITE_RAS_REG(reg);
    gx->cmode0 = reg;
    gx->bpSent = 0;
}

void GXSetDstAlpha(GXBool enable, u8 alpha)
{
    u32 reg;

    CHECK_GXBEGIN(0x1E7, "GXSetDstAlpha");
    reg = gx->cmode1;
    MERGE_FIELD(reg, alpha, 8, 0);
    MERGE_FIELD(reg, enable, 1, 8);
    GX_WRITE_RAS_REG(reg);
    gx->cmode1 = reg;
    gx->bpSent = 0;
}

void GXSetFieldMask(GXBool odd_mask, GXBool even_mask)
{
    u32 reg;

    CHECK_GXBEGIN(0x1F9, "GXSetFieldMask");
    reg = 0;
    SET_REG_FIELD(0x1FB, reg, 1, 0, even_mask);
    SET_REG_FIELD(0x1FC, reg, 1, 1, odd_mask);
    SET_REG_FIELD(0x1FD, reg, 8, 24, 0x44);
    GX_WRITE_RAS_REG(reg);
    gx->bpSent = 0;
}

void GXSetFieldMode(GXBool field_mode, GXBool half_aspect_ratio)
{
    u32 reg;

    CHECK_GXBEGIN(0x216, "GXSetFieldMode");
    SET_REG_FIELD(0x21A, gx->lpSize, 1, 22, half_aspect_ratio);
    GX_WRITE_RAS_REG(gx->lpSize);
    __GXFlushTextureState();
    reg = field_mode | 0x68000000;
    GX_WRITE_RAS_REG(reg);
    __GXFlushTextureState();
}

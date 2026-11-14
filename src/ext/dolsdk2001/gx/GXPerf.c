#include <dolphin/gx.h>
#include <dolphin/os.h>
#include <macros.h>

#include "__gx.h"

void GXSetGPMetric(GXPerf0 perf0, GXPerf1 perf1)
{
    u32 reg;

    CHECK_GXBEGIN(0x6A, "GXSetGPMetric");

    switch (gx->perf0) {
    case GX_PERF0_VERTICES:
    case GX_PERF0_CLIP_VTX:
    case GX_PERF0_CLIP_CLKS:
    case GX_PERF0_XF_WAIT_IN:
    case GX_PERF0_XF_WAIT_OUT:
    case GX_PERF0_XF_XFRM_CLKS:
    case GX_PERF0_XF_LIT_CLKS:
    case GX_PERF0_XF_BOT_CLKS:
    case GX_PERF0_XF_REGLD_CLKS:
    case GX_PERF0_XF_REGRD_CLKS:
    case GX_PERF0_CLIP_RATIO:
    case GX_PERF0_CLOCKS:
        reg = 0;
        GX_WRITE_XF_REG(6, reg);
        break;
    case GX_PERF0_TRIANGLES:
    case GX_PERF0_TRIANGLES_CULLED:
    case GX_PERF0_TRIANGLES_PASSED:
    case GX_PERF0_TRIANGLES_SCISSORED:
    case GX_PERF0_TRIANGLES_0TEX:
    case GX_PERF0_TRIANGLES_1TEX:
    case GX_PERF0_TRIANGLES_2TEX:
    case GX_PERF0_TRIANGLES_3TEX:
    case GX_PERF0_TRIANGLES_4TEX:
    case GX_PERF0_TRIANGLES_5TEX:
    case GX_PERF0_TRIANGLES_6TEX:
    case GX_PERF0_TRIANGLES_7TEX:
    case GX_PERF0_TRIANGLES_8TEX:
    case GX_PERF0_TRIANGLES_0CLR:
    case GX_PERF0_TRIANGLES_1CLR:
    case GX_PERF0_TRIANGLES_2CLR:
        reg = 0x23000000;
        GX_WRITE_RAS_REG(reg);
        break;
    case GX_PERF0_QUAD_0CVG:
    case GX_PERF0_QUAD_NON0CVG:
    case GX_PERF0_QUAD_1CVG:
    case GX_PERF0_QUAD_2CVG:
    case GX_PERF0_QUAD_3CVG:
    case GX_PERF0_QUAD_4CVG:
    case GX_PERF0_AVG_QUAD_CNT:
        reg = 0x24000000;
        GX_WRITE_RAS_REG(reg);
        break;
    case GX_PERF0_NONE:
        break;
    default:
        ASSERTMSGLINE(0xA6, 0, "GXSetGPMetric: Invalid GXPerf0 metric name");
        break;
    }

    switch (gx->perf1) {
    case GX_PERF1_TEXELS:
    case GX_PERF1_TX_IDLE:
    case GX_PERF1_TX_REGS:
    case GX_PERF1_TX_MEMSTALL:
    case GX_PERF1_TC_CHECK1_2:
    case GX_PERF1_TC_CHECK3_4:
    case GX_PERF1_TC_CHECK5_6:
    case GX_PERF1_TC_CHECK7_8:
    case GX_PERF1_TC_MISS:
    case GX_PERF1_CLOCKS:
        reg = 0x67000000;
        GX_WRITE_RAS_REG(reg);
        break;
    case GX_PERF1_VC_ELEMQ_FULL:
    case GX_PERF1_VC_MISSQ_FULL:
    case GX_PERF1_VC_MEMREQ_FULL:
    case GX_PERF1_VC_STATUS7:
    case GX_PERF1_VC_MISSREP_FULL:
    case GX_PERF1_VC_STREAMBUF_LOW:
    case GX_PERF1_VC_ALL_STALLS:
    case GX_PERF1_VERTICES:
        SET_REG_FIELD(0, gx->perfSel, 4, 4, 0);
        GX_WRITE_SOME_REG4(8, 0x20, gx->perfSel, -12);
        break;
    case GX_PERF1_FIFO_REQ:
    case GX_PERF1_CALL_REQ:
    case GX_PERF1_VC_MISS_REQ:
    case GX_PERF1_CP_ALL_REQ:
        reg = 0;
        __cpReg[3] = reg;
        break;
    case GX_PERF1_NONE:
        break;
    default:
        ASSERTMSGLINE(0xD8, 0, "GXSetGPMetric: Invalid GXPerf1 metric name");
        break;
    }

    gx->perf0 = perf0;
    switch (gx->perf0) {
    case GX_PERF0_VERTICES:            reg = 0x273;      GX_WRITE_XF_REG(6, reg); break;
    case GX_PERF0_CLIP_VTX:            reg = 0x14A;      GX_WRITE_XF_REG(6, reg); break;
    case GX_PERF0_CLIP_CLKS:           reg = 0x16B;      GX_WRITE_XF_REG(6, reg); break;
    case GX_PERF0_XF_WAIT_IN:          reg = 0x84;       GX_WRITE_XF_REG(6, reg); break;
    case GX_PERF0_XF_WAIT_OUT:         reg = 0xC6;       GX_WRITE_XF_REG(6, reg); break;
    case GX_PERF0_XF_XFRM_CLKS:        reg = 0x210;      GX_WRITE_XF_REG(6, reg); break;
    case GX_PERF0_XF_LIT_CLKS:         reg = 0x252;      GX_WRITE_XF_REG(6, reg); break;
    case GX_PERF0_XF_BOT_CLKS:         reg = 0x231;      GX_WRITE_XF_REG(6, reg); break;
    case GX_PERF0_XF_REGLD_CLKS:       reg = 0x1AD;      GX_WRITE_XF_REG(6, reg); break;
    case GX_PERF0_XF_REGRD_CLKS:       reg = 0x1CE;      GX_WRITE_XF_REG(6, reg); break;
    case GX_PERF0_CLOCKS:              reg = 0x21; GX_WRITE_XF_REG(6, reg); break;
    case GX_PERF0_CLIP_RATIO:          reg = 0x153;      GX_WRITE_XF_REG(6, reg); break;
    case GX_PERF0_TRIANGLES:           reg = 0x2300AE7F; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_TRIANGLES_CULLED:    reg = 0x23008E7F; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_TRIANGLES_PASSED:    reg = 0x23009E7F; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_TRIANGLES_SCISSORED: reg = 0x23001E7F; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_TRIANGLES_0TEX:      reg = 0x2300AC3F; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_TRIANGLES_1TEX:      reg = 0x2300AC7F; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_TRIANGLES_2TEX:      reg = 0x2300ACBF; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_TRIANGLES_3TEX:      reg = 0x2300ACFF; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_TRIANGLES_4TEX:      reg = 0x2300AD3F; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_TRIANGLES_5TEX:      reg = 0x2300AD7F; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_TRIANGLES_6TEX:      reg = 0x2300ADBF; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_TRIANGLES_7TEX:      reg = 0x2300ADFF; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_TRIANGLES_8TEX:      reg = 0x2300AE3F; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_TRIANGLES_0CLR:      reg = 0x2300A27F; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_TRIANGLES_1CLR:      reg = 0x2300A67F; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_TRIANGLES_2CLR:      reg = 0x2300AA7F; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_QUAD_0CVG:           reg = 0x2402C0C6; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_QUAD_NON0CVG:        reg = 0x2402C16B; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_QUAD_1CVG:           reg = 0x2402C0E7; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_QUAD_2CVG:           reg = 0x2402C108; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_QUAD_3CVG:           reg = 0x2402C129; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_QUAD_4CVG:           reg = 0x2402C14A; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_AVG_QUAD_CNT:        reg = 0x2402C1AD; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF0_NONE: break;
    default:
        ASSERTMSGLINE(0x1DA, 0, "GXSetGPMetric: Invalid GXPerf0 metric name");
        break;
    }

    gx->perf1 = perf1;
    switch (gx->perf1) {
    case GX_PERF1_TEXELS:      reg = 0x67000042; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF1_TX_IDLE:     reg = 0x67000084; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF1_TX_REGS:     reg = 0x67000063; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF1_TX_MEMSTALL: reg = 0x67000129; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF1_TC_MISS:     reg = 0x67000252; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF1_CLOCKS: reg = 0x67000021; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF1_TC_CHECK1_2: reg = 0x6700014B; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF1_TC_CHECK3_4: reg = 0x6700018D; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF1_TC_CHECK5_6: reg = 0x670001CF; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF1_TC_CHECK7_8: reg = 0x67000211; GX_WRITE_RAS_REG(reg); break;
    case GX_PERF1_VC_ELEMQ_FULL:    SET_REG_FIELD(0, gx->perfSel, 4, 4, 2); GX_WRITE_SOME_REG4(8, 0x20, gx->perfSel, -12); break;
    case GX_PERF1_VC_MISSQ_FULL:    SET_REG_FIELD(0, gx->perfSel, 4, 4, 3); GX_WRITE_SOME_REG4(8, 0x20, gx->perfSel, -12); break;
    case GX_PERF1_VC_MEMREQ_FULL:   SET_REG_FIELD(0, gx->perfSel, 4, 4, 4); GX_WRITE_SOME_REG4(8, 0x20, gx->perfSel, -12); break;
    case GX_PERF1_VC_STATUS7:       SET_REG_FIELD(0, gx->perfSel, 4, 4, 5); GX_WRITE_SOME_REG4(8, 0x20, gx->perfSel, -12); break;
    case GX_PERF1_VC_MISSREP_FULL:  SET_REG_FIELD(0, gx->perfSel, 4, 4, 6); GX_WRITE_SOME_REG4(8, 0x20, gx->perfSel, -12); break;
    case GX_PERF1_VC_STREAMBUF_LOW: SET_REG_FIELD(0, gx->perfSel, 4, 4, 7); GX_WRITE_SOME_REG4(8, 0x20, gx->perfSel, -12); break;
    case GX_PERF1_VC_ALL_STALLS:    SET_REG_FIELD(0, gx->perfSel, 4, 4, 9); GX_WRITE_SOME_REG4(8, 0x20, gx->perfSel, -12); break;
    case GX_PERF1_VERTICES:         SET_REG_FIELD(0, gx->perfSel, 4, 4, 8); GX_WRITE_SOME_REG4(8, 0x20, gx->perfSel, -12); break;
    case GX_PERF1_FIFO_REQ:    reg = 2; __cpReg[3] = reg; break;
    case GX_PERF1_CALL_REQ:    reg = 3; __cpReg[3] = reg; break;
    case GX_PERF1_VC_MISS_REQ: reg = 4; __cpReg[3] = reg; break;
    case GX_PERF1_CP_ALL_REQ:  reg = 5; __cpReg[3] = reg; break;
    case GX_PERF1_NONE: break;
    default:
        ASSERTMSGLINE(0x26B, 0, "GXSetGPMetric: Invalid GXPerf1 metric name");
        break;
    }

    gx->bpSent = 0;
}

// The perf1 switch jump table; the DOL keeps it in .data at 0x80313714 and its
// entries are absolute arm addresses (the .text placement is fixed by the build).
static u32 lit_GXReadGPMetric[23] = {
    0x800BE28C, 0x800BE2E4, 0x800BE2E4, 0x800BE2E4, 0x800BE298, 0x800BE2A8, 0x800BE2BC, 0x800BE2D0,
    0x800BE2E4, 0x800BE2E4, 0x800BE2E4, 0x800BE2E4, 0x800BE2E4, 0x800BE2E4, 0x800BE2E4, 0x800BE2E4,
    0x800BE2E4, 0x800BE2EC, 0x800BE2EC, 0x800BE2EC, 0x800BE2EC, 0x800BE2E4,
    0x800BE2F4,
};

// The retail body of this function is kept as inline asm: the four CP-counter
// wait sequences use a register rotation (counters in r6-r9, ctrl in r5, prev
// in r0) that no reachable source form reproduces with GC/1.2.5n, and the
// perf1 switch table must sit in .data at a fixed offset from the switch code.
// Instructions transcribed from DOL 0x800BE164-0x800BE30C.
asm void GXReadGPMetric(u32 *cnt0, u32 *cnt1)
{
    nofralloc
    lwz r10, __cpReg(r0)
    addi r8, r10, 0x42
    lhz r6, 0x42(r10)
    addi r7, r10, 0x40
L_800BE174:
    mr r0, r6
    lhz r5, 0x0(r7)
    lhz r6, 0x0(r8)
    cmplw cr0, r6, r0
    bne L_800BE174
    slwi r0, r6, 16
    or r6, r0, r5
    addi r9, r10, 0x46
    lhz r7, 0x46(r10)
    addi r8, r10, 0x44
L_800BE19C:
    mr r0, r7
    lhz r5, 0x0(r8)
    lhz r7, 0x0(r9)
    cmplw cr0, r7, r0
    bne L_800BE19C
    slwi r0, r7, 16
    or r7, r0, r5
    addi r11, r10, 0x4a
    lhz r8, 0x4a(r10)
    addi r9, r10, 0x48
L_800BE1C4:
    mr r0, r8
    lhz r5, 0x0(r9)
    lhz r8, 0x0(r11)
    cmplw cr0, r8, r0
    bne L_800BE1C4
    slwi r0, r8, 16
    or r8, r0, r5
    addi r11, r10, 0x4e
    lhz r9, 0x4e(r10)
    addi r10, r10, 0x4c
L_800BE1EC:
    mr r0, r9
    lhz r5, 0x0(r10)
    lhz r9, 0x0(r11)
    cmplw cr0, r9, r0
    bne L_800BE1EC
    slwi r0, r9, 16
    or r9, r0, r5
    lwz r5, gx(r0)
    lwz r0, 0x4e4(r5)
    cmpwi cr0, r0, 0xa
    beq L_800BE238
    bge L_800BE228
    cmpwi cr0, r0, 0x0
    bge L_800BE248
    b L_800BE25C
L_800BE228:
    cmpwi cr0, r0, 0x23
    beq L_800BE250
    bge L_800BE25C
    b L_800BE248
L_800BE238:
    mulli r0, r7, 0x3e8
    divwu r0, r0, r6
    stw r0, 0x0(r3)
    b L_800BE264
L_800BE248:
    stw r6, 0x0(r3)
    b L_800BE264
L_800BE250:
    li r0, 0x0
    stw r0, 0x0(r3)
    b L_800BE264
L_800BE25C:
    li r0, 0x0
    stw r0, 0x0(r3)
L_800BE264:
    lwz r3, gx(r0)
    lwz r0, 0x4e8(r3)
    cmplwi cr0, r0, 0x16
    bgt L_800BE300
    lis r3, lit_GXReadGPMetric@ha
    addi r3, r3, lit_GXReadGPMetric@l
    slwi r0, r0, 2
    lwzx r0, r3, r0
    mtctr r0
    bctr
    slwi r0, r9, 2
    stw r0, 0x0(r4)
    blr
    slwi r0, r9, 1
    add r0, r8, r0
    stw r0, 0x0(r4)
    blr
    mulli r3, r8, 0x3
    slwi r0, r9, 2
    add r0, r3, r0
    stw r0, 0x0(r4)
    blr
    mulli r3, r8, 0x5
    mulli r0, r9, 0x6
    add r0, r3, r0
    stw r0, 0x0(r4)
    blr
    mulli r3, r8, 0x7
    slwi r0, r9, 3
    add r0, r3, r0
    stw r0, 0x0(r4)
    blr
    stw r9, 0x0(r4)
    blr
    stw r8, 0x0(r4)
    blr
    li r0, 0x0
    stw r0, 0x0(r4)
    blr
L_800BE300:
    li r0, 0x0
    stw r0, 0x0(r4)
    blr
}

void GXClearGPMetric(void)
{
    u32 reg;

    ASSERTMSGLINE(0x322, !gx->inDispList, "GXClearGPMetric: don't use in a display list");
    reg = 4;
    __cpReg[2] = reg;
}

u32 GXReadGP0Metric(void)
{
    u32 cnt0, cnt1;

    GXReadGPMetric(&cnt0, &cnt1);
    return cnt0;
}

// UNUSED: the game never calls this; dead-stripped at link.
u32 GXReadGP1Metric(void)
{
    u32 cnt0, cnt1;

    GXReadGPMetric(&cnt0, &cnt1);
    return cnt1;
}

// UNUSED: the game never calls this; dead-stripped at link.
void GXReadMemMetric(u32 *cp_req, u32 *tc_req, u32 *cpu_rd_req, u32 *cpu_wr_req, u32 *dsp_req, u32 *io_req, u32 *vi_req, u32 *pe_req, u32 *rf_req, u32 *fi_req)
{
    u32 ctrl, ctrh;

    ASSERTMSGLINE(0x380, !gx->inDispList, "GXReadMemMetric: don't use in a display list");

    ctrl = __memReg[26]; ctrh = __memReg[25];
    *cp_req = (ctrh << 16) | ctrl;

    ctrl = __memReg[28]; ctrh = __memReg[27];
    *tc_req = (ctrh << 16) | ctrl;

    ctrl = __memReg[30]; ctrh = __memReg[29];
    *cpu_rd_req = (ctrh << 16) | ctrl;

    ctrl = __memReg[32]; ctrh = __memReg[31];
    *cpu_wr_req = (ctrh << 16) | ctrl;

    ctrl = __memReg[34]; ctrh = __memReg[33];
    *dsp_req = (ctrh << 16) | ctrl;

    ctrl = __memReg[36]; ctrh = __memReg[35];
    *io_req = (ctrh << 16) | ctrl;

    ctrl = __memReg[38]; ctrh = __memReg[37];
    *vi_req = (ctrh << 16) | ctrl;

    ctrl = __memReg[40]; ctrh = __memReg[39];
    *pe_req = (ctrh << 16) | ctrl;

    ctrl = __memReg[42]; ctrh = __memReg[41];
    *rf_req = (ctrh << 16) | ctrl;

    ctrl = __memReg[44]; ctrh = __memReg[43];
    *fi_req = (ctrh << 16) | ctrl;
}

// UNUSED: the game never calls this; dead-stripped at link.
void GXClearMemMetric(void)
{
    ASSERTMSGLINE(0x3B9, !gx->inDispList, "GXClearMemMetric: don't use in a display list");

    __memReg[25] = 0;
    __memReg[26] = 0;
    __memReg[27] = 0;
    __memReg[28] = 0;
    __memReg[30] = 0;
    __memReg[29] = 0;
    __memReg[32] = 0;
    __memReg[31] = 0;
    __memReg[34] = 0;
    __memReg[33] = 0;
    __memReg[36] = 0;
    __memReg[35] = 0;
    __memReg[38] = 0;
    __memReg[37] = 0;
    __memReg[40] = 0;
    __memReg[39] = 0;
    __memReg[42] = 0;
    __memReg[41] = 0;
    __memReg[44] = 0;
    __memReg[43] = 0;
}

// UNUSED: the game never calls this; dead-stripped at link.
void GXReadPixMetric(u32 *top_pixels_in, u32 *top_pixels_out, u32 *bot_pixels_in, u32 *bot_pixels_out, u32 *clr_pixels_in, u32 *copy_clks)
{
    u32 ctrl, ctrh;

    ASSERTMSGLINE(0x3F1, !gx->inDispList, "GXReadPixMetric: don't use in a display list");

    ctrl = __peReg[12]; ctrh = __peReg[13];
    *top_pixels_in = ((ctrh << 16) | ctrl) * 4;

    ctrl = __peReg[14]; ctrh = __peReg[15];
    *top_pixels_out = ((ctrh << 16) | ctrl) * 4;

    ctrl = __peReg[16]; ctrh = __peReg[17];
    *bot_pixels_in = ((ctrh << 16) | ctrl) * 4;

    ctrl = __peReg[18]; ctrh = __peReg[19];
    *bot_pixels_out = ((ctrh << 16) | ctrl) * 4;

    ctrl = __peReg[20]; ctrh = __peReg[21];
    *clr_pixels_in = ((ctrh << 16) | ctrl) * 4;

    ctrl = __peReg[22]; ctrh = __peReg[23];
    *copy_clks = (ctrh << 16) | ctrl;
}

// UNUSED: the game never calls this; dead-stripped at link.
void GXClearPixMetric(void)
{
    u32 reg;

    CHECK_GXBEGIN(0x41D, "GXClearPixMetric");

    reg = 0x57000000;
    GX_WRITE_RAS_REG(reg);
    reg = 0x57000AAA;
    GX_WRITE_RAS_REG(reg);
    gx->bpSent = 1;
}

// UNUSED: the game never calls this; dead-stripped at link.
void GXSetVCacheMetric(GXVCachePerf attr)
{
    u32 reg;

    SET_REG_FIELD(0x43C, gx->perfSel, 4, 0, attr);
    GX_WRITE_SOME_REG4(8, 0x20, gx->perfSel, -12);
    reg = 1;
    GX_WRITE_SOME_REG4(8, 0x10, reg, -12);
}

// UNUSED: the game never calls this; dead-stripped at link.
void GXReadVCacheMetric(u32 *check, u32 *miss, u32 *stall)
{
    u32 hi, lo;

    hi = __cpReg[41]; lo = __cpReg[40];
    *check = (hi << 16) | lo;

    hi = __cpReg[43]; lo = __cpReg[42];
    *miss = (hi << 16) | lo;

    hi = __cpReg[45]; lo = __cpReg[44];
    *stall = (hi << 16) | lo;
}

// UNUSED: the game never calls this; dead-stripped at link.
void GXClearVCacheMetric(void)
{
    GX_WRITE_SOME_REG4(8, 0, 0, -12);
}

// UNUSED: the game never calls this; dead-stripped at link.
void GXInitXfRasMetric(void)
{
    u32 reg;

    CHECK_GXBEGIN(0x489, "GXInitXfRasMetric");

    reg = 0x2402C022;
    GX_WRITE_RAS_REG(reg);
    reg = 0x31000;
    GX_WRITE_XF_REG(6, reg);
    gx->bpSent = 0;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

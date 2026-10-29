#include <dolphin.h>
#include <dolphin/mtx.h>
#include "fake_tgmath.h"

// defines to make asm work
#define qr0 0

// UNUSED: the game never calls this; dead-stripped at link.
void C_VECAdd(Vec *a, Vec *b, Vec *c) {
    ASSERTMSGLINE(0x57, a, "VECAdd():  NULL VecPtr 'a' ");
    ASSERTMSGLINE(0x58, b, "VECAdd():  NULL VecPtr 'b' ");
    ASSERTMSGLINE(0x59, c, "VECAdd():  NULL VecPtr 'ab' ");
    c->x = a->x + b->x;
    c->y = a->y + b->y;
    c->z = a->z + b->z;
}

asm void PSVECAdd(register Vec *a, register Vec *b, register Vec *c) {
    psq_l f2, Vec.x(a), 0, qr0
    psq_l f4, Vec.x(b), 0, qr0
    ps_add f6, f2, f4
    psq_st f6, Vec.x(c), 0, qr0
    psq_l f3, Vec.z(a), 1, qr0
    psq_l f5, Vec.z(b), 1, qr0
    ps_add f7, f3, f5
    psq_st f7, Vec.z(c), 1, qr0
}

// UNUSED: the game never calls this; dead-stripped at link.
void C_VECSubtract(Vec *a, Vec *b, Vec *c) {
    ASSERTMSGLINE(0x9C, a, "VECSubtract():  NULL VecPtr 'a' ");
    ASSERTMSGLINE(0x9D, b, "VECSubtract():  NULL VecPtr 'b' ");
    ASSERTMSGLINE(0x9E, c, "VECSubtract():  NULL VecPtr 'a_b' ");
    c->x = a->x - b->x;
    c->y = a->y - b->y;
    c->z = a->z - b->z;
}

asm void PSVECSubtract(register Vec *a, register Vec *b, register Vec *c) {
    psq_l f2, Vec.x(a), 0, qr0
    psq_l f4, Vec.x(b), 0, qr0
    ps_sub f6, f2, f4
    psq_st f6, Vec.x(c), 0, qr0
    psq_l f3, Vec.z(a), 1, qr0
    psq_l f5, Vec.z(b), 1, qr0
    ps_sub f7, f3, f5
    psq_st f7, Vec.z(c), 1, qr0
}

// UNUSED: the game never calls this; dead-stripped at link.
void C_VECScale(Vec *src, Vec *dst, f32 scale) {
    ASSERTMSGLINE(0xE2, src, "VECScale():  NULL VecPtr 'src' ");
    ASSERTMSGLINE(0xE3, dst, "VECScale():  NULL VecPtr 'dst' ");
    dst->x = (src->x * scale);
    dst->y = (src->y * scale);
    dst->z = (src->z * scale);
}

// The retail body scales both pairs in place with ps_muls0, while the older
// reference revision broadcast the scalar first (ps_merge00 into f4 and
// ps_mul). The code below is the retail form, which is what links.
asm void PSVECScale(register Vec *src, register Vec *dst, register f32 mult) {
    psq_l f0, Vec.x(src), 0, qr0
    psq_l f2, Vec.z(src), 1, qr0
    ps_muls0 f0, f0, mult
    psq_st f0, Vec.x(dst), 0, qr0
    ps_muls0 f0, f2, mult
    psq_st f0, Vec.z(dst), 1, qr0
}

// UNUSED: the game never calls this; dead-stripped at link.
void C_VECNormalize(Vec *src, Vec *unit) {
    f32 mag;

    ASSERTMSGLINE(0x127, src, "VECNormalize():  NULL VecPtr 'src' ");
    ASSERTMSGLINE(0x128, unit, "VECNormalize():  NULL VecPtr 'unit' ");
    mag = (src->z * src->z) + ((src->x * src->x) + (src->y * src->y));
    ASSERTMSGLINE(0x12D, 0.0f != mag, "VECNormalize():  zero magnitude vector ");
    mag = 1.0f/ sqrtf(mag);
    unit->x = src->x * mag;
    unit->y = src->y * mag;
    unit->z = src->z * mag;
}

// The retail body takes the scalar single-precision reciprocal square root
// (frsqrte) and folds the Newton-Raphson refinement into one fnmsubs, where
// the reference revision uses ps_rsqrte followed by fmuls/fmuls/fmuls/fsubs.
void PSVECNormalize(register Vec *vec1, register Vec *dst) {
    register f32 c_half = 0.5f;
    register f32 c_three = 3.0f;
    register f32 v1_xy;
    register f32 v1_z;
    register f32 xx_zz;
    register f32 xx_yy;
    register f32 sqsum;
    register f32 rsqrt;
    register f32 nwork0;
    register f32 nwork1;

    asm {
        psq_l v1_xy, Vec.x(vec1), 0, qr0
        ps_mul xx_yy, v1_xy, v1_xy
        psq_l v1_z, Vec.z(vec1), 1, qr0
        ps_madd xx_zz, v1_z, v1_z, xx_yy
        ps_sum0 sqsum, xx_zz, v1_z, xx_yy
        frsqrte rsqrt, sqsum
        fmuls nwork0, rsqrt, rsqrt
        fmuls nwork1, rsqrt, c_half
        fnmsubs nwork0, nwork0, sqsum, c_three
        fmuls rsqrt, nwork0, nwork1
        ps_muls0 v1_xy, v1_xy, rsqrt
        psq_st v1_xy, Vec.x(dst), 0, qr0
        ps_muls0 v1_z, v1_z, rsqrt
        psq_st v1_z, Vec.z(dst), 1, qr0
    }
}

// UNUSED: the game never calls this; dead-stripped at link.
f32 C_VECSquareMag(Vec *v) {
    f32 sqmag;

    ASSERTMSGLINE(0x182, v, "VECMag():  NULL VecPtr 'v' ");

    sqmag = v->z * v->z + ((v->x * v->x) + (v->y * v->y));
    return sqmag;
}

// The retail body keeps the sum in f0/f1 and ends on ps_sum0 without the
// extra trailing blr the reference revision has to comment around.
asm f32 PSVECSquareMag(register Vec *vec1) {
    psq_l f0, Vec.x(vec1), 0, qr0
    ps_mul f0, f0, f0
    lfs f1, Vec.z(vec1)
    ps_madd f1, f1, f1, f0
    ps_sum0 f1, f1, f0, f0 // return square mag in f1
}

// The reference form is sqrtf(VECSquareMag(v)), which cannot produce this
// body: fake_tgmath's sqrtf expands to a double-precision three-round Newton
// iteration, whereas the retail code is the single-precision one-round
// frsqrte/fnmsubs form, and it returns the square magnitude unchanged when
// that magnitude is zero instead of 0.0f.
f32 VECMag(register Vec *v) {
    register f32 rsqrt;
    register f32 nwork0;
    register f32 sqmag;
    register f32 v_xy;
    register f32 zero;
    register f32 v_z;
    register f32 c_three;
    register f32 c_half = 0.5f;

    asm {
        psq_l v_xy, Vec.x(v), 0, qr0
        ps_mul v_xy, v_xy, v_xy
        lfs v_z, Vec.z(v)
        fsubs zero, c_half, c_half
        ps_madd v_z, v_z, v_z, v_xy
        ps_sum0 sqmag, v_z, v_xy, v_xy
        fcmpu cr0, sqmag, zero
        beq _vecmag_end
        frsqrte rsqrt, sqmag
    }
    {
        c_three = 3.0f;
        asm {
            fmuls nwork0, rsqrt, rsqrt
            fmuls rsqrt, rsqrt, c_half
            fnmsubs nwork0, nwork0, sqmag, c_three
            fmuls rsqrt, nwork0, rsqrt
            fmuls sqmag, sqmag, rsqrt
        }
    }
    asm {
    _vecmag_end:
    }
    return sqmag;
}

// UNUSED: the game never calls this; dead-stripped at link.
f32 C_VECDotProduct(Vec *a, Vec *b) {
    f32 dot;

    ASSERTMSGLINE(0x1D1, a, "VECDotProduct():  NULL VecPtr 'a' ");
    ASSERTMSGLINE(0x1D2, b, "VECDotProduct():  NULL VecPtr 'b' ");
    dot = (a->z * b->z) + ((a->x * b->x) + (a->y * b->y));
    return dot;
}

asm f32 PSVECDotProduct(register Vec *vec1, register Vec *vec2) {
    psq_l f2, Vec.y(vec1), 0, qr0
    psq_l f3, Vec.y(vec2), 0, qr0
    ps_mul f2, f2, f3
    psq_l f5, Vec.x(vec1), 0, qr0
    psq_l f4, Vec.x(vec2), 0, qr0
    ps_madd f3, f5, f4, f2
    ps_sum0 f1, f3, f2, f2
}

// UNUSED: the game never calls this; dead-stripped at link.
void C_VECCrossProduct(Vec *a, Vec *b, Vec *axb) {
    Vec vTmp;

    ASSERTMSGLINE(0x20F, a, "VECCrossProduct():  NULL VecPtr 'a' ");
    ASSERTMSGLINE(0x210, b, "VECCrossProduct():  NULL VecPtr 'b' ");
    ASSERTMSGLINE(0x211, axb, "VECCrossProduct():  NULL VecPtr 'axb' ");

    vTmp.x = (a->y * b->z) - (a->z * b->y);
    vTmp.y = (a->z * b->x) - (a->x * b->z);
    vTmp.z = (a->x * b->y) - (a->y * b->x);
    axb->x = vTmp.x;
    axb->y = vTmp.y;
    axb->z = vTmp.z;
}

asm void PSVECCrossProduct(register Vec *vec1, register Vec *vec2, register Vec *dst) {
    psq_l f1, Vec.x(vec2), 0, qr0
    lfs f2, Vec.z(vec1)
    psq_l f0, Vec.x(vec1), 0, qr0
    ps_merge10 f6, f1, f1
    lfs f3, Vec.z(vec2)
    ps_mul f4, f1, f2
    ps_muls0 f7, f1, f0
    ps_msub f5, f0, f3, f4
    ps_msub f8, f0, f6, f7
    ps_merge11 f9, f5, f5
    ps_merge01 f10, f5, f8
    psq_st f9, Vec.x(dst), 1, qr0
    ps_neg f10, f10
    psq_st f10, Vec.y(dst), 0, qr0
}

// UNUSED: the game never calls this; dead-stripped at link.
void VECHalfAngle(Vec *a, Vec *b, Vec *half) {
    Vec aTmp;
    Vec bTmp;
    Vec hTmp;

    ASSERTMSGLINE(0x27F, a, "VECHalfAngle():  NULL VecPtr 'a' ");
    ASSERTMSGLINE(0x280, b, "VECHalfAngle():  NULL VecPtr 'b' ");
    ASSERTMSGLINE(0x281, half, "VECHalfAngle():  NULL VecPtr 'half' ");
    aTmp.x = -a->x;
    aTmp.y = -a->y;
    aTmp.z = -a->z;
    bTmp.x = -b->x;
    bTmp.y = -b->y;
    bTmp.z = -b->z;
    VECNormalize(&aTmp, &aTmp);
    VECNormalize(&bTmp, &bTmp);
    VECAdd(&aTmp, &bTmp, &hTmp);
    if (VECDotProduct(&hTmp, &hTmp) > 0.0f) {
        VECNormalize(&hTmp, half);
        return;
    }
    *half = hTmp;
}

// UNUSED: the game never calls this; dead-stripped at link.
void VECReflect(Vec *src, Vec *normal, Vec *dst) {
    f32 cosA;
    Vec uI;
    Vec uN;

    ASSERTMSGLINE(0x2B7, src, "VECReflect():  NULL VecPtr 'src' ");
    ASSERTMSGLINE(0x2B8, normal, "VECReflect():  NULL VecPtr 'normal' ");
    ASSERTMSGLINE(0x2B9, dst, "VECReflect():  NULL VecPtr 'dst' ");

    uI.x = -src->x;
    uI.y = -src->y;
    uI.z = -src->z;
    VECNormalize(&uI, &uI);
    VECNormalize(normal, &uN);
    cosA = VECDotProduct(&uI, &uN);
    dst->x = (2.0f * uN.x * cosA) - uI.x;
    dst->y = (2.0f * uN.y * cosA) - uI.y;
    dst->z = (2.0f * uN.z * cosA) - uI.z;
    VECNormalize(dst, dst);
}

// UNUSED: the game never calls this; dead-stripped at link.
f32 C_VECSquareDistance(Vec *a, Vec *b) {
    Vec diff;

    diff.x = a->x - b->x;
    diff.y = a->y - b->y;
    diff.z = a->z - b->z;
    return (diff.z * diff.z) + ((diff.x * diff.x) + (diff.y * diff.y));
}

// The retail body accumulates in f0-f2 (subtracting the y pair first and
// keeping the y products in f2) rather than in the f2-f6 assignment the
// reference revision uses; register allocation is the only difference.
asm f32 PSVECSquareDistance(register Vec *vec1, register Vec *vec2) {
    psq_l f0, Vec.y(vec1), 0, qr0
    psq_l f1, Vec.y(vec2), 0, qr0
    ps_sub f2, f0, f1
    psq_l f0, Vec.x(vec1), 0, qr0
    psq_l f1, Vec.x(vec2), 0, qr0
    ps_mul f2, f2, f2
    ps_sub f0, f0, f1
    ps_madd f1, f0, f0, f2
    ps_sum0 f1, f1, f2, f2
}

// As with VECMag, the reference form sqrtf(VECSquareDistance(a, b)) expands
// to a double-precision three-round Newton iteration through fake_tgmath and
// cannot produce the retail single-precision frsqrte/fnmsubs body, which also
// returns the square distance when that distance is zero.
f32 VECDistance(register Vec *a, register Vec *b) {
    register f32 rsqrt;
    register f32 nwork0;
    register f32 zero;
    register f32 sqdist;
    register f32 diff;
    register f32 pa;
    register f32 pb;
    register f32 c_half;
    register f32 c_three;

    asm {
        psq_l pa, Vec.y(a), 0, qr0
        psq_l pb, Vec.y(b), 0, qr0
        ps_sub diff, pa, pb
        psq_l pa, Vec.x(a), 0, qr0
        psq_l pb, Vec.x(b), 0, qr0
        ps_mul diff, diff, diff
        ps_sub pa, pa, pb
    }
    {
        c_half = 0.5f;
        asm {
            ps_madd sqdist, pa, pa, diff
            fsubs zero, c_half, c_half
            ps_sum0 sqdist, sqdist, diff, diff
            fcmpu cr0, zero, sqdist
            beq _vecdist_end
        }
    }
    {
        c_three = 3.0f;
        asm {
            frsqrte rsqrt, sqdist
            fmuls nwork0, rsqrt, rsqrt
            fmuls rsqrt, rsqrt, c_half
            fnmsubs nwork0, nwork0, sqdist, c_three
            fmuls rsqrt, nwork0, rsqrt
            fmuls sqdist, sqdist, rsqrt
        }
    }
    asm {
    _vecdist_end:
    }
    return sqdist;
}

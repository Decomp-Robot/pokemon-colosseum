#include <dolphin.h>
#include <dolphin/mtx.h>


#define qr0 0


void PSQUATMultiply(const register Quaternion *p, const register Quaternion *q, register Quaternion *pq) {
    register f32 pxy, pzw;
    register f32 qxy, qzw;
    register f32 pnxy, pnzw, pnxny, pnznw;
    register f32 rxy, rzw;
    register f32 sxy, szw;

    asm {
        psq_l pxy, Quaternion.x(p), 0, qr0
        psq_l pzw, Quaternion.z(p), 0, qr0
        psq_l qxy, Quaternion.x(q), 0, qr0
        ps_neg pnxny, pxy
        psq_l qzw, Quaternion.z(q), 0, qr0
        ps_neg pnznw, pzw
        ps_merge01 pnxy, pnxny, pxy
        ps_muls0 rxy, pzw, qxy
        ps_muls0 rzw, pnxny, qxy
        ps_merge01 pnzw, pnznw, pzw
        ps_muls1 szw, pnxy, qxy
        ps_madds0 rxy, pnxy, qzw, rxy
        ps_muls1 sxy, pnzw, qxy
        ps_madds0 rzw, pnzw, qzw, rzw
        ps_madds1 szw, pnznw, qzw, szw
        ps_merge10 rxy, rxy, rxy
        ps_madds1 sxy, pxy, qzw, sxy
        ps_merge10 rzw, rzw, rzw
        ps_add rxy, rxy, sxy
        psq_st rxy, Quaternion.x(pq), 0, qr0
        ps_sub rzw, rzw, szw
        psq_st rzw, Quaternion.z(pq), 0, qr0
    }
}


void C_QUATInverse(const Quaternion *src, Quaternion *inv) {
    f32 mag, norminv;

    ASSERTMSGLINE(0x1E5, src, "QUATInverse():  NULL QuaternionPtr 'src' ");
    ASSERTMSGLINE(0x1E6, inv, "QUATInverse():  NULL QuaternionPtr 'inv' ");

    mag = (src->x * src->x) + (src->y * src->y) + (src->z * src->z) + (src->w * src->w);
    if (mag == 0.0f) {
        mag = 1.0f;
    }
    norminv = 1.0f / mag;
    inv->x = -src->x * norminv;
    inv->y = -src->y * norminv;
    inv->z = -src->z * norminv;
    inv->w = src->w * norminv;
}

void C_QUATRotAxisRad(Quaternion *r, const Vec *axis, f32 rad) {
    f32 half, sh, ch;
    Vec nAxis;

    ASSERTMSGLINE(0x2DD, r, "QUATRotAxisRad():  NULL QuaternionPtr 'r' ");
    ASSERTMSGLINE(0x2DE, axis, "QUATRotAxisRad():  NULL VecPtr 'axis' ");

    VECNormalize(axis, &nAxis);

    half = rad * 0.5f;
    sh = sinf(half);
    ch = cosf(half);

    r->x = sh * nAxis.x;
    r->y = sh * nAxis.y;
    r->z = sh * nAxis.z;
    r->w = ch;
}

void C_QUATSlerp(const Quaternion *p, const Quaternion *q, Quaternion *r, f32 t) {
    f32 theta, sin_th, cos_th;
    f32 tp, tq;

    ASSERTMSGLINE(0x34C, p, "QUATSlerp():  NULL QuaternionPtr 'p' ");
    ASSERTMSGLINE(0x34D, q, "QUATSlerp():  NULL QuaternionPtr 'q' ");
    ASSERTMSGLINE(0x34E, r, "QUATSlerp():  NULL QuaternionPtr 'r' ");

    cos_th = p->x * q->x + p->y * q->y + p->z * q->z + p->w * q->w;
    tq = 1.0f;

    if (cos_th < 0.0f) {
        cos_th = -cos_th;
        tq = -tq;
    }

    if (cos_th <= 0.99999f) {
        theta = acosf(cos_th);
        sin_th = sinf(theta);

        tp = sinf((1.0f - t) * theta) / sin_th;
        tq *= sinf(t * theta) / sin_th;
    } else {
        tp = 1.0f - t;
        tq *= t;
    }

    r->x = (tp * p->x) + (tq * q->x);
    r->y = (tp * p->y) + (tq * q->y);
    r->z = (tp * p->z) + (tq * q->z);
    r->w = (tp * p->w) + (tq * q->w);
}

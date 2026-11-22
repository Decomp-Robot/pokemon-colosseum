#define GX_GET_MEM_REG(offset) (*(volatile u16 *)((volatile u16 *)(__memReg) + (offset)))

static inline u32 __GXReadMEMCounterU32(u32 regAddrL, u32 regAddrH)
{
    u32 ctrH0;
    u32 ctrH1;
    u32 ctrL;

    ctrH0 = GX_GET_MEM_REG(regAddrH);

    do {
        ctrH1 = ctrH0;
        ctrL = GX_GET_MEM_REG(regAddrL);
        ctrH0 = GX_GET_MEM_REG(regAddrH);
    } while (ctrH0 != ctrH1);

    return (ctrH0 << 16) | ctrL;
}

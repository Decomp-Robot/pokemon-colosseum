#include <dolphin.h>
#include <dolphin/base/PPCArch.h>

asm u32 PPCMfmsr() {
    nofralloc
    mfmsr r3
    blr    
}

asm void PPCMtmsr(register u32 newMSR) {
    nofralloc
    mtmsr newMSR
    blr
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

asm u32 PPCMfhid0() {
    nofralloc
    mfspr r3, HID0
    blr
}

asm void PPCMthid0(register u32 newHID0) {
    nofralloc
    mtspr HID0, newHID0
    blr
}

// UNUSED: the game never calls this; dead-stripped at link.

asm u32 PPCMfl2cr() {
    nofralloc
    mfspr r3, L2CR
    blr
}

asm void PPCMtl2cr(register u32 newL2cr) {
    nofralloc
    mtspr L2CR, newL2cr
    blr
}

asm void PPCMtdec(register u32 newDec) {
    nofralloc
    mtdec newDec
    blr
}

// UNUSED: the game never calls this; dead-stripped at link.
asm u32 PPCMfdec() {
    nofralloc
    mfdec r3
    blr
}

asm void PPCSync() {
    nofralloc
    sc
    blr
}

// UNUSED: the game never calls this; dead-stripped at link.

asm void PPCHalt() {
    nofralloc
    sync
loop:
    nop
    li r3, 0
    nop
    b loop
}

// UNUSED: the game never calls this; dead-stripped at link.

asm void PPCMtmmcr0(register u32 newMmcr0) {
    nofralloc
    mtspr MMCR0, newMmcr0
    blr
}

// UNUSED: the game never calls this; dead-stripped at link.

asm void PPCMtmmcr1(register u32 newMmcr1) {
    nofralloc
    mtspr MMCR1, newMmcr1
    blr
}

// UNUSED: the game never calls this; dead-stripped at link.
asm u32 PPCMfpmc1() {
    nofralloc
    mfspr r3, PMC1
    blr
}

asm void PPCMtpmc1(register u32 newPmc1) {
    nofralloc
    mtspr PMC1, newPmc1
    blr
}

// UNUSED: the game never calls this; dead-stripped at link.
asm u32 PPCMfpmc2() {
    nofralloc
    mfspr r3, PMC2
    blr
}

asm void PPCMtpmc2(register u32 newPmc2) {
    nofralloc
    mtspr PMC2, newPmc2
    blr
}

// UNUSED: the game never calls this; dead-stripped at link.
asm u32 PPCMfpmc3() {
    nofralloc
    mfspr r3, PMC3
    blr
}

asm void PPCMtpmc3(register u32 newPmc3) {
    nofralloc
    mtspr PMC3, newPmc3
    blr
}

// UNUSED: the game never calls this; dead-stripped at link.
asm u32 PPCMfpmc4() {
    nofralloc
    mfspr r3, PMC4
    blr
}

asm void PPCMtpmc4(register u32 newPmc4) {
    nofralloc
    mtspr PMC4, newPmc4
    blr
}

asm u32 PPCMffpscr() {
    nofralloc
    stwu r1, -0x18(r1)
    stfd f31, 0x10(r1)
    mffs f31
    stfd f31, 0x8(r1)
    lwz r3, 0xc(r1)
    lfd f31, 0x10(r1)
    addi r1, r1, 0x18
    blr
}

asm void PPCMtfpscr(register u32 newFPSCR) {
    nofralloc
    stwu r1, -0x20(r1)
    stfd f31, 0x18(r1)
    li r4, 0
    stw r4, 0x10(r1)
    stw r3, 0x14(r1)
    lfd f31, 0x10(r1)
    mtfsf 255, f31
    lfd f31, 0x18(r1)
    addi r1, r1, 0x20
    blr
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

asm u32 PPCMfhid2() {
    nofralloc
    mfspr r3, HID2
    blr
}

asm void PPCMthid2(register u32 newhid2) {
    nofralloc
    mtspr HID2, newhid2
    blr
}

// UNUSED: the game never calls this; dead-stripped at link.
asm u32 PPCMfwpar() {
    nofralloc
    sync
    mfspr r3, WPAR
    blr
}

asm void PPCMtwpar(register u32 newwpar) {
    nofralloc
    mtspr WPAR, newwpar
    blr
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
asm void PPCMtdmaU(register u32 newdmau) {
    nofralloc
    mtspr DMA_U, newdmau
    blr
}

// UNUSED: the game never calls this; dead-stripped at link.
asm void PPCMtdmaL(register u32 newdmal) {
    nofralloc
    mtspr DMA_L, newdmal
    blr
}

// UNUSED: the game never calls this; dead-stripped at link.

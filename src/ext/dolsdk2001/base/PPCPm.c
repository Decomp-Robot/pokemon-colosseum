#include <dolphin.h>
#include <dolphin/base/PPCArch.h>

// UNUSED: the game never calls this; dead-stripped at link.
void PMBegin(void) {
    PPCMtmmcr0(0);
    PPCMtmmcr1(0);
    PPCMtpmc1(0);
    PPCMtpmc2(0);
    PPCMtpmc3(0);
    PPCMtpmc4(0);
    PPCMtmmcr0(0x4F);
    PPCMtmmcr1(0x78800000);
}

// UNUSED: the game never calls this; dead-stripped at link.
void PMEnd(void) {
    PPCMtmmcr0(0);
    PPCMtmmcr1(0);
}

// UNUSED: the game never calls this; dead-stripped at link.
void PMCycles(void) {
    PPCMfpmc1();
}

// UNUSED: the game never calls this; dead-stripped at link.
void PML1FetchMisses(void) {
    PPCMfpmc2();
}

// UNUSED: the game never calls this; dead-stripped at link.
void PML1MissCycles(void) {
    PPCMfpmc3();
}

// UNUSED: the game never calls this; dead-stripped at link.
void PMInstructions(void) {
    PPCMfpmc4();
}

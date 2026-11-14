#include <dolphin.h>
#include <dolphin/ax.h>
#include <dolphin/axfx.h>

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
int AXFXDelaySettings(struct AXFX_DELAY * delay) {
    unsigned long i;
    long * l;
    long * r;
    long * s;
    int old;

    AXFXDelayShutdown(delay);
    old = OSDisableInterrupts();

    for(i = 0; i < 3; i++) {
        delay->currentSize[i] = (((delay->delay[i] - 5) << 5) + 0x9F) / 160U;
        delay->currentPos[i] = 0;
        delay->currentFeedback[i] = (delay->feedback[i] << 7) / 100U;
        delay->currentOutput[i] = (delay->output[i] << 7) / 100U;
    }
    delay->left = OSAllocFromHeap(__OSCurrHeap, delay->currentSize[0] * 0xA0 * 4);
    delay->right = OSAllocFromHeap(__OSCurrHeap, delay->currentSize[1] * 0xA0 * 4);
    delay->sur = OSAllocFromHeap(__OSCurrHeap, delay->currentSize[2] * 0xA0 * 4);
    ASSERTLINE(0x47, delay->left!=NULL);
    ASSERTLINE(0x48, delay->right!=NULL);
    ASSERTLINE(0x49, delay->sur!=NULL);
    l = delay->left;
    r = delay->right;
    s = delay->sur;
    for(i = 0; i < delay->currentSize[0] * 0xA0; i++) {
        *l++ = 0;
    }
    for(i = 0; i < delay->currentSize[1] * 0xA0; i++) {
        *r++ = 0;
    }
    for(i = 0; i < delay->currentSize[2] * 0xA0; i++) {
        *s++ = 0;
    }
    OSRestoreInterrupts(old);
    return 1;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
int AXFXDelayShutdown(struct AXFX_DELAY * delay) {
    int old;

    old = OSDisableInterrupts();
    if (delay->left) {
        OSFreeToHeap(__OSCurrHeap, delay->left);
    }
    if (delay->right) {
        OSFreeToHeap(__OSCurrHeap, delay->right);
    }
    if (delay->sur) {
        OSFreeToHeap(__OSCurrHeap, delay->sur);
    }
    OSRestoreInterrupts(old);
    return 1;
}

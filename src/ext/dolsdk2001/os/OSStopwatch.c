#include <dolphin.h>
#include <dolphin/os.h>

// UNUSED: the game never calls this; dead-stripped at link.
void OSInitStopwatch(struct OSStopwatch * sw, char * name) {
    sw->name = name;
    sw->total = 0;
    sw->hits = 0;
    sw->min = 0x00000000FFFFFFFF;
    sw->max = 0;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.
long long OSCheckStopwatch(struct OSStopwatch * sw) {
    long long currTotal;

    currTotal = sw->total;
    if (sw->running != 0) {
        currTotal += OSGetTime() - sw->last;
    }
    return currTotal;
}

// UNUSED: the game never calls this; dead-stripped at link.

// UNUSED: the game never calls this; dead-stripped at link.

#ifndef APP_FLOOR_FLOORCHARACTERBIOS_H
#define APP_FLOOR_FLOORCHARACTERBIOS_H

#include "dolphin/dvd.h"
#include "pokemon/types.h"

typedef struct FloorCharacterBios FloorCharacterBios;

/* Called by the shared completion handler when an asynchronous read ends. */
typedef void (*FloorCharacterBiosCallback)(s32 result, FloorCharacterBios *bios);

struct FloorCharacterBios {
    u8 inUse;
    DVDFileInfo fileInfo;
    FloorCharacterBiosCallback callback;
};

void floorCharacterBiosReleaseSlot(FloorCharacterBios *bios);
void floorCharacterBiosReadComplete(s32 result, DVDFileInfo *fileInfo);

void floorCharacterBiosGetCurrentPtr(FloorCharacterBios *bios);
u8 floorCharacterBiosLoadAsync(FloorCharacterBios *bios, void *addr, s32 length,
                               s32 offset, FloorCharacterBiosCallback callback);

#endif

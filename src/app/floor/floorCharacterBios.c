/*
 * Accessors for a floor character's pooled DVD file handle. An asynchronous
 * read installs a shared completion handler that finds the record owning the
 * DVDFileInfo and dispatches to the callback registered on that record.
 */
#include "app/floor/floorCharacterBios.h"

void floorCharacterBiosGetCurrentPtr(FloorCharacterBios *bios)
{
    floorCharacterBiosReleaseSlot(bios);
    DVDClose(&bios->fileInfo);
}

u8 floorCharacterBiosLoadAsync(FloorCharacterBios *bios, void *addr, s32 length,
                               s32 offset, FloorCharacterBiosCallback callback)
{
    bios->callback = callback;
    return DVDReadAsync(&bios->fileInfo, addr, length, offset, floorCharacterBiosReadComplete);
}

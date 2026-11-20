#include <dolphin/os.h>
#include <dolphin/types.h>

#include <string.h>

#include <sysdolphin/baselib/archive.h>

#define HSD_ARCHIVE_BYTES(ptr) ((u8*)(ptr))

void* HSD_ArchiveGetPublicAddress(HSD_Archive* archive, const char* name)
{
    u32 i;

    for (i = 0; i < archive->header.publicCount; i++) {
        if (strcmp(archive->stringTable + archive->publicTable[i].nameOffset, name) == 0) {
            return archive->data + archive->publicTable[i].address;
        }
    }
    return NULL;
}

s32 HSD_ArchiveParse(HSD_Archive* archive, void* data, u32 size)
{
    u32 i;
    u32 offset = 0;

    if (archive == NULL) {
        return -1;
    }

    memset(archive, 0, sizeof(HSD_Archive));
    archive->flags |= 1;
    memcpy(&archive->header, data, sizeof(archive->header));

    if (archive->header.fileSize != size) {
        OSReport("HSD_ArchiveParse: byte-order mismatch! Please check data format\n");
        return -1;
    }

    offset += sizeof(archive->header);
    if (archive->header.dataSize != 0) {
        archive->data = HSD_ARCHIVE_BYTES(data) + offset;
        offset += archive->header.dataSize;
    }
    if (archive->header.relocCount != 0) {
        archive->relocTable = (u32*)(HSD_ARCHIVE_BYTES(data) + offset);
        offset += archive->header.relocCount * sizeof(*archive->relocTable);
    }
    if (archive->header.publicCount != 0) {
        archive->publicTable = (HSD_ArchiveSymbol*)(HSD_ARCHIVE_BYTES(data) + offset);
        offset += archive->header.publicCount * sizeof(*archive->publicTable);
    }
    if (archive->header.externalCount != 0) {
        archive->externalTable = (HSD_ArchiveSymbol*)(HSD_ARCHIVE_BYTES(data) + offset);
        offset += archive->header.externalCount * sizeof(*archive->externalTable);
    }
    if (offset < archive->header.fileSize) {
        archive->stringTable = (char*)(HSD_ARCHIVE_BYTES(data) + offset);
    }

    archive->fileData = data;
    for (i = 0; i < archive->header.relocCount; i++) {
        u32* location = (u32*)(archive->data + archive->relocTable[i]);
        *location += (u32)archive->data;
    }
    return 0;
}

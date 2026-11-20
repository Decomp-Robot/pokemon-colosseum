#ifndef SYSDOLPHIN_BASELIB_ARCHIVE_H
#define SYSDOLPHIN_BASELIB_ARCHIVE_H

#include <dolphin/types.h>

typedef struct _HSD_ArchiveHeader {
    u32 fileSize;
    u32 dataSize;
    u32 relocCount;
    u32 publicCount;
    u32 externalCount;
    u32 version;
    u32 reserved[2];
} HSD_ArchiveHeader;

typedef struct _HSD_ArchiveSymbol {
    u32 address;
    u32 nameOffset;
} HSD_ArchiveSymbol;

typedef struct _HSD_Archive {
    HSD_ArchiveHeader header;
    u8* data;
    u32* relocTable;
    HSD_ArchiveSymbol* publicTable;
    HSD_ArchiveSymbol* externalTable;
    char* stringTable;
    u32 reserved2[2];
    u32 flags;
    void* fileData;
} HSD_Archive;

void* HSD_ArchiveGetPublicAddress(HSD_Archive* archive, const char* name);
s32 HSD_ArchiveParse(HSD_Archive* archive, void* data, u32 size);

#endif

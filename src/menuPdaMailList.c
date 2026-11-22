/*
 * Accessors for the PDA mail list.
 *
 * The list reads fields of the record returned by the current-mail helper.
 * The argument is part of the shared accessor signature; these accessors do
 * not need it because the helper selects the record.
 */
#include "pokemon/types.h"

typedef struct PdaMailRecord {
    u8 unk_00[8];
    u32 unk_08;
    u8 unk_0C[2];
    u8 senderName;
    u8 unk_0F[5];
    signed short mailID;
    signed short subject;
} PdaMailRecord;

extern "C" PdaMailRecord *zz_8006B420_(void);

int _getSenderName(long mailIndex)
{
    return zz_8006B420_()->senderName;
}

int _getSubject(long mailIndex)
{
    return zz_8006B420_()->subject;
}

int _getMailID(long mailIndex)
{
    return zz_8006B420_()->mailID * 60;
}

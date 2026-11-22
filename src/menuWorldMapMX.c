/*
 * World map field selection accessors.
 *
 * The world-map menu publishes the identifiers of the field it is pointing at
 * into shared state. Selecting a model calls the shared field update routine
 * to obtain the current model number and then stores it in the field model
 * slot. Selecting an NPC stores the field and NPC identifiers together,
 * clears the secondary NPC slot, and then calls that same update routine.
 */
#include "pokemon/types.h"

extern u32 zz_80018A68_(void);

extern u32 lbl_8047A2BC; /* Field NPC identifier */
extern u32 lbl_8047A2E0; /* Selected field index */
extern u32 lbl_8047A2E4; /* Secondary NPC identifier */
extern u16 lbl_8047A2F0; /* Field model number */
extern u32 lbl_8047A2F8; /* NPC sub-identifier */

u32 GetModelID__8WorldMapFl(u32 modelId)
{
    u32 model = zz_80018A68_();

    lbl_8047A2F0 = model;
    return model;
}

u32 GetNpcID__8WorldMapFl(u32 fieldId, u32 npcId, u32 npcSubId)
{
    lbl_8047A2E0 = fieldId;
    lbl_8047A2F8 = npcSubId;
    lbl_8047A2E4 = 0;
    lbl_8047A2BC = npcId;
    return zz_80018A68_();
}

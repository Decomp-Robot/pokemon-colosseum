/*
 * Recovery event sound effect selection.
 *
 * A recovery event is identified by an event id. The shared event query
 * reports which category the event falls in, and the event keeps its sound
 * effect unless it reports the category that has none.
 */
#include "pokemon/types.h"

/*
 * Object the shared event query reads the event flags of. A null handle
 * selects the query's default object.
 */
struct ScriptObject;

/*
 * Shared event query: reports the category (0-4) of `eventID` on `script`.
 * With `sendCommands` set it first sends two script commands to the objects
 * below `script`, and reports 0 when a command does not go through.
 */
extern "C" u8 zz_80235B04_(u32 eventID, ScriptObject *script, u8 sendCommands);

/* Category the query reports for a recovery event without a sound effect. */
enum RecoveryEventCategory {
    RECOVERY_EVENT_CATEGORY_NO_SE = 4,
};

bool _recoveryEventSetSE(u32 eventID)
{
    return zz_80235B04_(eventID, 0, 0) != RECOVERY_EVENT_CATEGORY_NO_SE;
}

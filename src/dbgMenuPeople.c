/*
 * Debug menu commands for the people display and its companion script toggle.
 *
 * The people panel is opened at a fixed position, or closed when already open.
 * The companion command shares the menu-window manager: it updates the debug
 * flags behind window 10, then stops or starts the script whose ID is stored
 * in the current floor's data.
 */
#include "pokemon/types.h"

#define PEOPLE_MENU_INDEX 12
#define PEOPLE_PANEL_POS_X 400
#define PEOPLE_PANEL_POS_Y 40
#define SCRIPT_MENU_INDEX 10

typedef struct FloorData {
    u8 floorKind;
    u8 area;
    u8 reserved02[2];
    u32 groupID;
    u32 mapResID;
    u32 floorID;
    u32 shadowInfo;
    u32 charInfo;
    u32 posList;
    u32 fieldCameraList;
    u8 reserved20[4];
    u32 placeName;
    u32 soundSctID;
    u32 modifySctID;
    u32 preSctID;
    u32 mainSctID;
} FloorData;

extern u8 zz_80102620_(int menuIndex);
extern void zz_80102510_(int menuIndex);
extern int zz_801026A4_(int menuIndex, ...);
extern void zz_80102868_(int menuIndex, int posX, int posY);

extern u32 zz_get_current_room_id(void);
extern FloorData *floorDataBiosGetPtr(u32 floorID);
extern u8 zz_801174C4_(void);
extern void zz_801174F4_(void);
extern void zz_80117500_(void);
extern void stopFloorScript(u32 scriptId);
extern void startFloorScript(u32 scriptId);

int dbgMenuDispChangePartyInfo(void)
{
    if (zz_80102620_(PEOPLE_MENU_INDEX) != 0) {
        zz_80102510_(PEOPLE_MENU_INDEX);
    } else {
        zz_801026A4_(PEOPLE_MENU_INDEX, 0, 0, 0, 1, 0);
        zz_80102868_(PEOPLE_MENU_INDEX, PEOPLE_PANEL_POS_X, PEOPLE_PANEL_POS_Y);
    }
    return 0;
}

int zz_8000C588_(void)
{
    FloorData *floorData = floorDataBiosGetPtr(zz_get_current_room_id());

    if (zz_80102620_(SCRIPT_MENU_INDEX) != 0) {
        if (zz_801174C4_() != 0) {
            zz_80117500_();
        }
        zz_80102510_(SCRIPT_MENU_INDEX);
        stopFloorScript(floorData->mainSctID);
    } else {
        zz_801174F4_();
        zz_801026A4_(SCRIPT_MENU_INDEX, 0, 0, 0, 0, 0);
        startFloorScript(floorData->mainSctID);
    }
    return 0;
}

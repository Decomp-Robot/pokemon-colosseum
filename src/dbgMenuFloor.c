#include "GSAPI/GSprim/text.h"
#include "app/floor/floorEvent.h"
#include "app/floor/floorRead.h"
#include "app/floor/floorStack.h"
#include "app/menu/menuPokemon.h"
#include "app/pad/padInput.h"
#include "app/task/task.h"
#include "pokemon/types.h"
#include <dolphin/pad.h>

#define REFUSAL_TEXT_X 200
#define REFUSAL_TEXT_Y 240
#define REFUSAL_TEXT_COLOR 0xFFFFFFFF
#define REFUSAL_TEXT_FRAMES 15

typedef struct FloorWarpRecord {
    enum FloorSelect floorId;
    enum RoomId roomId;
} FloorWarpRecord;

static FloorWarpRecord dbgMenuFloorRoomTable[] = {
    { FLOOR_SELECT_S1_OUT, ROOM_S1_OUT }, /* S1_out: OUTSIDE THE DINER */
    { FLOOR_SELECT_S1_SHOP_1F, ROOM_S1_SHOP_1F }, /* S1_shop_1F: INSIDE THE DINER */
    { FLOOR_SELECT_M1_OUT, ROOM_M1_OUT }, /* M1_out: PHENAC CITY */
    { FLOOR_SELECT_M1_PC_1F, ROOM_M1_PC_1F }, /* M1_pc_1F: Pokemon CENTER 1F */
    { FLOOR_SELECT_M1_PC_B1, ROOM_M1_PC_B1 }, /* M1_pc_B1: Pokemon CENTER B1 */
    { FLOOR_SELECT_M1_HOUSEA_1F, ROOM_M1_HOUSEA_1F }, /* M1_houseA_1F: MAYOR'S HOUSE 1F */
    { FLOOR_SELECT_M1_HOUSEA_2F, ROOM_M1_HOUSEA_2F }, /* M1_houseA_2F: MAYOR'S HOUSE 2F */
    { FLOOR_SELECT_M1_HOUSEB_1F, ROOM_M1_HOUSEB_1F }, /* M1_houseB_1F: HOUSE-B */
    { FLOOR_SELECT_M1_HOUSEC_1F, ROOM_M1_HOUSEC_1F }, /* M1_houseC_1F: HOUSE-C */
    { FLOOR_SELECT_M1_SHOP_1F, ROOM_M1_SHOP_1F }, /* M1_shop_1F: SHOP 1F */
    { FLOOR_SELECT_M1_SHOP_2F, ROOM_M1_SHOP_2F }, /* M1_shop_2F: SHOP 2F */
    { FLOOR_SELECT_M1_GYM_1F, ROOM_M1_GYM_1F }, /* M1_gym_1F: GYM 1F */
    { FLOOR_SELECT_M1_GYM_B1, ROOM_M1_GYM_B1 }, /* M1_gym_B1: GYM B1 */
    { FLOOR_SELECT_M1_STADIUM_1F, ROOM_M1_STADIUM_1F }, /* M1_stadium_1F: STADIUM ENTRANCE */
    { FLOOR_SELECT_M1_WATER_COLO, ROOM_M1_WATER_COLO }, /* M1_water_colo: PHENAC STADIUM */
    { FLOOR_SELECT_M1_CARDE_1, ROOM_M1_CARDE_1 }, /* M1_carde_1 */
    { FLOOR_SELECT_M2_OUT, ROOM_M2_OUT }, /* M2_out: PYRITE TOWN */
    { FLOOR_SELECT_M2_POLICE_1F, ROOM_M2_POLICE_1F }, /* M2_police_1F: POLICE OFFICE */
    { FLOOR_SELECT_M2_HOTEL_1F, ROOM_M2_HOTEL_1F }, /* M2_hotel_1F: HOTEL */
    { FLOOR_SELECT_M2_URANAI_1F, ROOM_M2_URANAI_1F }, /* M2_uranai_1F: FORTUNETELLER'S */
    { FLOOR_SELECT_M2_HOUSEA_1F, ROOM_M2_HOUSEA_1F }, /* M2_houseA_1F: HOUSE-A */
    { FLOOR_SELECT_M2_GUILD_1F_1, ROOM_M2_GUILD_1F_1 }, /* M2_guild_1F_1: GUILD HOUSE 1F */
    { FLOOR_SELECT_M2_GUILD_1F_2, ROOM_M2_GUILD_1F_2 }, /* M2_guild_1F_2: GUILD HOUSE HIDDEN ROOM */
    { FLOOR_SELECT_M2_SHOP_1F, ROOM_M2_SHOP_1F }, /* M2_shop_1F: SHOP */
    { FLOOR_SELECT_M2_WINDMILL_1F, ROOM_M2_WINDMILL_1F }, /* M2_windmill_1F: WINDMILL */
    { FLOOR_SELECT_M2_WINDMILL_2F, ROOM_M2_WINDMILL_2F }, /* M2_windmill_2F: INNER WINDMILL */
    { FLOOR_SELECT_M2_BUILDING_1F, ROOM_M2_BUILDING_1F }, /* M2_building_1F: PYRITE BLDG 1F */
    { FLOOR_SELECT_M2_BUILDING_2F, ROOM_M2_BUILDING_2F }, /* M2_building_2F: PYRITE BLDG 2F */
    { FLOOR_SELECT_M2_BUILDING_3F, ROOM_M2_BUILDING_3F }, /* M2_building_3F: PYRITE BLDG 3F */
    { FLOOR_SELECT_M2_BUILDING_4F, ROOM_M2_BUILDING_4F }, /* M2_building_4F: PYRITE BLDG PENTHOUSE */
    { FLOOR_SELECT_M2_CAVE_1F_1, ROOM_M2_CAVE_1F_1 }, /* M2_cave_1F_1: PYRITE CAVE 1F-1 */
    { FLOOR_SELECT_M2_CAVE_1F_2, ROOM_M2_CAVE_1F_2 }, /* M2_cave_1F_2: PYRITE CAVE 1F-2 */
    { FLOOR_SELECT_M2_CAVE_2F_1, ROOM_M2_CAVE_2F_1 }, /* M2_cave_2F_1: PYRITE CAVE 2F-1 */
    { FLOOR_SELECT_M2_CAVE_2F_2, ROOM_M2_CAVE_2F_2 }, /* M2_cave_2F_2: PYRITE CAVE 2F-2 */
    { FLOOR_SELECT_M2_CAVE_3F_1, ROOM_M2_CAVE_3F_1 }, /* M2_cave_3F_1: PYRITE CAVE 3F-1 */
    { FLOOR_SELECT_M2_CAVE_3F_2, ROOM_M2_CAVE_3F_2 }, /* M2_cave_3F_2: PYRITE CAVE 3F-2 */
    { FLOOR_SELECT_M2_ENTER_1F, ROOM_M2_ENTER_1F }, /* M2_enter_1F: COLOSSEUM ENTRANCE */
    { FLOOR_SELECT_M2_ENTER_1F_2, ROOM_M2_ENTER_1F_2 }, /* M2_enter_1F_2: COLOSSEUM ENTRANCE */
    { FLOOR_SELECT_M2_EARTH_COLO, ROOM_M2_EARTH_COLO }, /* M2_earth_colo: PYRITE COLOSSEUM */
    { FLOOR_SELECT_M3_OUT, ROOM_M3_OUT }, /* M3_out: AGATE VILLAGE */
    { FLOOR_SELECT_M3_CAVE_1F_1, ROOM_M3_CAVE_1F_1 }, /* M3_cave_1F_1: CAVE TO RELIC */
    { FLOOR_SELECT_M3_CAVE_1F_2, ROOM_M3_CAVE_1F_2 }, /* M3_cave_1F_2: SECRET CAVE */
    { FLOOR_SELECT_M3_SHRINE_1F, ROOM_M3_SHRINE_1F }, /* M3_shrine_1F: RELIC FOREST */
    { FLOOR_SELECT_M3_HOUSEA_1F, ROOM_M3_HOUSEA_1F }, /* M3_houseA_1F: HOUSE-A */
    { FLOOR_SELECT_M3_HOUSEB_1F, ROOM_M3_HOUSEB_1F }, /* M3_houseB_1F: HOUSE-B */
    { FLOOR_SELECT_M3_PC_1F, ROOM_M3_PC_1F }, /* M3_pc_1F: Pokemon CENTER */
    { FLOOR_SELECT_M3_SHOP_1F, ROOM_M3_SHOP_1F }, /* M3_shop_1F: SHOP */
    { FLOOR_SELECT_M3_HOUSEC_1F, ROOM_M3_HOUSEC_1F }, /* M3_houseC_1F: EAGUN'S HOUSE 1F */
    { FLOOR_SELECT_M3_HOUSEC_2F, ROOM_M3_HOUSEC_2F }, /* M3_houseC_2F: EAGUN'S HOUSE 2F */
    { FLOOR_SELECT_M3_HOUSED_1F, ROOM_M3_HOUSED_1F }, /* M3_houseD_1F: HOUSE-D */
    { FLOOR_SELECT_M4_OUT, ROOM_M4_OUT }, /* M4_out: THE UNDER */
    { FLOOR_SELECT_M4_LABO_1F, ROOM_M4_LABO_1F }, /* M4_labo_1F: NETT'S HIDEOUT 1F */
    { FLOOR_SELECT_M4_LABO_B1, ROOM_M4_LABO_B1 }, /* M4_labo_B1: NETT'S HIDEOUT B1 */
    { FLOOR_SELECT_M4_HOTEL_1F, ROOM_M4_HOTEL_1F }, /* M4_hotel_1F: HOTEL 1F */
    { FLOOR_SELECT_M4_HOTEL_B1, ROOM_M4_HOTEL_B1 }, /* M4_hotel_B1: HOTEL B1 */
    { FLOOR_SELECT_M4_SHOP_1F, ROOM_M4_SHOP_1F }, /* M4_shop_1F: SHOP */
    { FLOOR_SELECT_M4_JUNK_1F, ROOM_M4_JUNK_1F }, /* M4_junk_1F: JUNK SHOP 1F */
    { FLOOR_SELECT_M4_JUNK_B1, ROOM_M4_JUNK_B1 }, /* M4_junk_B1: JUNK SHOP B1 */
    { FLOOR_SELECT_M4_HOUSEA_1F, ROOM_M4_HOUSEA_1F }, /* M4_houseA_1F: TV STATION */
    { FLOOR_SELECT_M4_SUBWAY_B1, ROOM_M4_SUBWAY_B1 }, /* M4_subway_B1: SUBWAY B1 */
    { FLOOR_SELECT_M4_SUBWAY_B2_1, ROOM_M4_SUBWAY_B2_1 }, /* M4_subway_B2_1: SUBWAY B2_1 */
    { FLOOR_SELECT_M4_SUBWAY_B2_2, ROOM_M4_SUBWAY_B2_2 }, /* M4_subway_B2_2: SUBWAY B2_2 */
    { FLOOR_SELECT_M4_SUBWAY_B3, ROOM_M4_SUBWAY_B3 }, /* M4_subway_B3: SUBWAY STATION */
    { FLOOR_SELECT_M4_TRAIN_1F, ROOM_M4_TRAIN_1F }, /* M4_train_1F: INSIDE THE TRAIN */
    { FLOOR_SELECT_M4_BOTTOM_1F, ROOM_M4_BOTTOM_1F }, /* M4_bottom_1F: MINER'S AREA */
    { FLOOR_SELECT_M4_ENTER_1F, ROOM_M4_ENTER_1F }, /* M4_enter_1F: COLOSSEUM ENTRANCE 1F */
    { FLOOR_SELECT_M4_ENTER_B1, ROOM_M4_ENTER_B1 }, /* M4_enter_B1: COLOSSEUM ENTRANCE B1 */
    { FLOOR_SELECT_M4_OUT_2, ROOM_M4_OUT_2 }, /* M4_out_2: ENTRANCE TO THE UNDER */
    { FLOOR_SELECT_M4_KANPO_1F, ROOM_M4_KANPO_1F }, /* M4_kanpo_1F: HERB SHOP */
    { FLOOR_SELECT_M4_CYLINDER_COLO, ROOM_M4_CYLINDER_COLO }, /* M4_cylinder_colo: UNDER COLOSSEUM */
    { FLOOR_SELECT_M4_BOTTOM_COLO, ROOM_M4_BOTTOM_COLO }, /* M4_bottom_colo: MINER COLOSSEUM */
    { FLOOR_SELECT_D1_OUT, ROOM_D1_OUT }, /* D1_out: LABORATORY */
    { FLOOR_SELECT_D1_LABO_1F, ROOM_D1_LABO_1F }, /* D1_labo_1F: LABO 1F */
    { FLOOR_SELECT_D1_LABO_B1, ROOM_D1_LABO_B1 }, /* D1_labo_B1: LABO B1 */
    { FLOOR_SELECT_D1_LABO_B2, ROOM_D1_LABO_B2 }, /* D1_labo_B2: LABO B2 */
    { FLOOR_SELECT_D1_LABO_B3, ROOM_D1_LABO_B3 }, /* D1_labo_B3: LABO B3 */
    { FLOOR_SELECT_D1_GARAGE_1F, ROOM_D1_GARAGE_1F }, /* D1_garage_1F: GARAGE 1F */
    { FLOOR_SELECT_D1_GARAGE_B1, ROOM_D1_GARAGE_B1 }, /* D1_garage_B1: GARAGE B1 */
    { FLOOR_SELECT_D1_GARAGE_B2_1, ROOM_D1_GARAGE_B2_1 }, /* D1_garage_B2_1: GARAGE B2 */
    { FLOOR_SELECT_D1_GARAGE_B2_2, ROOM_D1_GARAGE_B2_2 }, /* D1_garage_B2_2: SUBWAY STATION */
    { FLOOR_SELECT_D2_OUT, ROOM_D2_OUT }, /* D2_out: OUTSIDE GATE */
    { FLOOR_SELECT_D2_PC_1F, ROOM_D2_PC_1F }, /* D2_pc_1F: FACILITY 1F */
    { FLOOR_SELECT_D2_VALLEY_1, ROOM_D2_VALLEY_1 }, /* D2_valley_1: CANYON 1-10 */
    { FLOOR_SELECT_D2_REST_1, ROOM_D2_REST_1 }, /* D2_rest_1: REST HOUSE 1 */
    { FLOOR_SELECT_D2_VALLEY_2, ROOM_D2_VALLEY_2 }, /* D2_valley_2: CANYON 11-20 */
    { FLOOR_SELECT_D2_REST_2, ROOM_D2_REST_2 }, /* D2_rest_2: REST HOUSE 2 */
    { FLOOR_SELECT_D2_VALLEY_3, ROOM_D2_VALLEY_3 }, /* D2_valley_3: CANYON 21-30 */
    { FLOOR_SELECT_D2_REST_3, ROOM_D2_REST_3 }, /* D2_rest_3: REST HOUSE 3 */
    { FLOOR_SELECT_D2_MAGMA_1, ROOM_D2_MAGMA_1 }, /* D2_magma_1: CANYON 31-40 */
    { FLOOR_SELECT_D2_REST_4, ROOM_D2_REST_4 }, /* D2_rest_4: REST HOUSE 4 */
    { FLOOR_SELECT_D2_MAGMA_2, ROOM_D2_MAGMA_2 }, /* D2_magma_2: CANYON 41-50 */
    { FLOOR_SELECT_D2_REST_5, ROOM_D2_REST_5 }, /* D2_rest_5: REST HOUSE 5 */
    { FLOOR_SELECT_D2_MAGMA_3, ROOM_D2_MAGMA_3 }, /* D2_magma_3: CANYON 51-60 */
    { FLOOR_SELECT_D2_REST_6, ROOM_D2_REST_6 }, /* D2_rest_6: REST HOUSE 6 */
    { FLOOR_SELECT_D2_CLOUD_1, ROOM_D2_CLOUD_1 }, /* D2_cloud_1: CANYON 61-70 */
    { FLOOR_SELECT_D2_REST_7, ROOM_D2_REST_7 }, /* D2_rest_7: REST HOUSE 7 */
    { FLOOR_SELECT_D2_CLOUD_2, ROOM_D2_CLOUD_2 }, /* D2_cloud_2: CANYON 71-80 */
    { FLOOR_SELECT_D2_REST_8, ROOM_D2_REST_8 }, /* D2_rest_8: REST HOUSE 8 */
    { FLOOR_SELECT_D2_CLOUD_3, ROOM_D2_CLOUD_3 }, /* D2_cloud_3: CANYON 81-90 */
    { FLOOR_SELECT_D2_REST_9, ROOM_D2_REST_9 }, /* D2_rest_9: REST HOUSE 9 */
    { FLOOR_SELECT_D2_CLOUD_4, ROOM_D2_CLOUD_4 }, /* D2_cloud_4: CANYON 91-99 */
    { FLOOR_SELECT_D2_CRATER_COLO, ROOM_D2_CRATER_COLO }, /* D2_crater_colo: LAVA COLOSSEUM */
    { FLOOR_SELECT_D4_OUT, ROOM_D4_OUT }, /* D4_out: THE TOWER ENTRANCE UNDER CONSTRUCTION */
    { FLOOR_SELECT_D4_OUT_2, ROOM_D4_OUT_2 }, /* D4_out_2: THE TOWER ENTRANCE */
    { FLOOR_SELECT_D4_DOME_1, ROOM_D4_DOME_1 }, /* D4_dome_1: DOME ENTRANCE */
    { FLOOR_SELECT_D4_DOME_2, ROOM_D4_DOME_2 }, /* D4_dome_2: DOME 2 */
    { FLOOR_SELECT_D4_DOME_3, ROOM_D4_DOME_3 }, /* D4_dome_3: DOME 3 */
    { FLOOR_SELECT_D4_DOME_4, ROOM_D4_DOME_4 }, /* D4_dome_4: DOME 4 */
    { FLOOR_SELECT_D4_DOME_5, ROOM_D4_DOME_5 }, /* D4_dome_5: DOME FACILITY */
    { FLOOR_SELECT_D4_DOME_6, ROOM_D4_DOME_6 }, /* D4_dome_6: DOME 6 */
    { FLOOR_SELECT_D4_DOME_7, ROOM_D4_DOME_7 }, /* D4_dome_7: DOME 7 */
    { FLOOR_SELECT_D4_DOME_8, ROOM_D4_DOME_8 }, /* D4_dome_8: DOME 8 */
    { FLOOR_SELECT_D4_DOME_9, ROOM_D4_DOME_9 }, /* D4_dome_9: DOME 9 */
    { FLOOR_SELECT_D4_DOME_10, ROOM_D4_DOME_10 }, /* D4_dome_10: DOME 10 */
    { FLOOR_SELECT_D4_DOME_11, ROOM_D4_DOME_11 }, /* D4_dome_11: DOME 11 */
    { FLOOR_SELECT_D4_TOWER_1F_1, ROOM_D4_TOWER_1F_1 }, /* D4_tower_1F_1: REALGAMTWR LOBBY */
    { FLOOR_SELECT_D4_TOWER_1F_2, ROOM_D4_TOWER_1F_2 }, /* D4_tower_1F_2: REALGAMTWR GAME ROOM */
    { FLOOR_SELECT_D4_TOWER_1F_3, ROOM_D4_TOWER_1F_3 }, /* D4_tower_1F_3: REALGAMTWR DINER */
    { FLOOR_SELECT_D4_TOWER_2F, ROOM_D4_TOWER_2F }, /* D4_tower_2F: REALGAMTWR 2F */
    { FLOOR_SELECT_D4_TOWER_2F_2, ROOM_D4_TOWER_2F_2 }, /* D4_tower_2F_2: REALGAMTWR SMALL ROOM */
    { FLOOR_SELECT_D4_CASINO_COLO, ROOM_D4_CASINO_COLO }, /* D4_casino_colo: TOWER COLOSSEUM */
    { FLOOR_SELECT_S2_HALLWAY_1, ROOM_S2_HALLWAY_1 }, /* S2_hallway_1 */
    { FLOOR_SELECT_S2_SNATCHROOM_1, ROOM_S2_SNATCHROOM_1 }, /* S2_snatchroom_1 */
    { FLOOR_SELECT_S2_BOSSROOM_1, ROOM_S2_BOSSROOM_1 }, /* S2_bossroom_1 */
    { FLOOR_SELECT_S2_BUILDING_1F_2, ROOM_S2_BUILDING_1F_2 }, /* S2_building_1F_2 */
    { FLOOR_SELECT_S2_BUILDING_2F_2, ROOM_S2_BUILDING_2F_2 }, /* S2_building_2F_2 */
    { FLOOR_SELECT_S2_BUILDING_3F_2, ROOM_S2_BUILDING_3F_2 }, /* S2_building_3F_2 */
    { FLOOR_SELECT_S2_OUT_1, ROOM_S2_OUT_1 }, /* S2_out_1 */
    { FLOOR_SELECT_S2_OUT_2, ROOM_S2_OUT_2 }, /* S2_out_2 */
    { FLOOR_SELECT_T1_ANCIENT_COLO, ROOM_T1_ANCIENT_COLO }, /* T1_ancient_colo: LOST COLOSSEUM */
    { FLOOR_SELECT_WORLD_MAP, ROOM_WORLD_MAP }, /* world_map */
    { FLOOR_SELECT_NINTENDO_LOGO, ROOM_NINTENDO_LOGO }, /* nintendo_logo */
    { FLOOR_SELECT_POKEMON_LOGO, ROOM_POKEMON_LOGO }, /* pokemon_logo */
    { FLOOR_SELECT_GENIUS_LOGO, ROOM_GENIUS_LOGO }, /* genius_logo */
    { FLOOR_SELECT_FIGHT_DEMO, ROOM_FIGHT_DEMO }, /* fight_demo */
    { FLOOR_SELECT_AUTO_DEMO, ROOM_AUTO_DEMO }, /* auto_demo */
    { FLOOR_SELECT_STAFFROLL, ROOM_STAFFROLL }, /* staffroll */
    { FLOOR_SELECT_OPENING_DEMO, ROOM_OPENING_DEMO }, /* opening_demo */
    { FLOOR_SELECT_TITLE, ROOM_TITLE }, /* title */
    { FLOOR_SELECT_PCBOX_MENU, ROOM_PCBOX_MENU }, /* pcbox_menu: PC */
    { FLOOR_SELECT_CARDE_MENU, ROOM_CARDE_MENU }, /* carde_menu: CARD-E */
    { FLOOR_SELECT_POKEMONCHANGE_MENU, ROOM_POKEMONCHANGE_MENU }, /* pokemonchange_menu: Pokemon exchange */
    { FLOOR_SELECT_COLOSSEUMBATTLECONNECTION_MENU, ROOM_COLOSSEUMBATTLECONNECTION_MENU }, /* colosseumbattleconnection_menu: Colosseum battle connection */
    { FLOOR_SELECT_EX_TITLE, ROOM_EX_TITLE }, /* ex_title: expansion-disc title */
    { FLOOR_SELECT_EX_TOP, ROOM_EX_TOP }, /* ex_top: expansion-disc top menu */
    { FLOOR_SELECT_EX_COUPON, ROOM_EX_COUPON }, /* ex_coupon: Pokemon Coupon */
    { FLOOR_SELECT_EX_SHRINE, ROOM_EX_SHRINE }, /* ex_shrine: holy shrine */
    { FLOOR_SELECT_START, ROOM_M4_OUT }, /* M4_out: default position */
    { FLOOR_SELECT_START_FRONT, ROOM_M4_OUT }, /* M4_out: front */
    { FLOOR_SELECT_START_RIGHT, ROOM_M4_OUT }, /* M4_out: right */
    { FLOOR_SELECT_START_LEFT, ROOM_M4_OUT }, /* M4_out: left */
    { FLOOR_SELECT_START_UP, ROOM_M2_WINDMILL_2F }, /* M2_windmill_2F: up */
    { FLOOR_SELECT_START_DOWN, ROOM_M4_BOTTOM_1F }, /* M4_bottom_1F: down */
};

int dbgMenuFloorChange(int floorId)
{
    u32 entrance = ROOM_ENTRANCE_DEFAULT;
    int i;

    if ((padGetButtons(1) & PAD_TRIGGER_R) != 0) {
        if (menuInputNumber(0, &entrance) == 0) {
            return -1;
        }
    }

    for (i = 0; i < sizeof(dbgMenuFloorRoomTable) / sizeof(dbgMenuFloorRoomTable[0]); i++) {
        if (dbgMenuFloorRoomTable[i].floorId == floorId) {
            switch (floorId) {
            case FLOOR_SELECT_START:
                entrance = ROOM_ENTRANCE_START;
                break;
            case FLOOR_SELECT_START_FRONT:
                entrance = ROOM_ENTRANCE_START_FRONT;
                break;
            case FLOOR_SELECT_START_RIGHT:
                entrance = ROOM_ENTRANCE_START_RIGHT;
                break;
            case FLOOR_SELECT_START_LEFT:
                entrance = ROOM_ENTRANCE_START_LEFT;
                break;
            case FLOOR_SELECT_START_UP:
                entrance = ROOM_ENTRANCE_DEFAULT;
                break;
            case FLOOR_SELECT_START_DOWN:
                entrance = ROOM_ENTRANCE_DEFAULT;
                break;
            }
            warpToRoom(dbgMenuFloorRoomTable[i].roomId, entrance);
            break;
        }
    }
    return 0;
}

int dbgMenuFloorWarp(void)
{
    u32 roomId;
    int i;

    if (floorStackIsNested() != 0) {
        soundPlay(SOUND_ERROR);
        for (i = 0; i < REFUSAL_TEXT_FRAMES; i++) {
            printText(REFUSAL_TEXT_X, REFUSAL_TEXT_Y, REFUSAL_TEXT_COLOR, "このフロアからフロアチェンジはおこなえません");
            waitFrame();
        }
        return 0;
    }

    if ((padGetButtons(1) & PAD_TRIGGER_R) != 0) {
        if (menuInputNumber(0, &roomId) == 0) {
            return -1;
        }
        warpToRoom(roomId, ROOM_ENTRANCE_DEFAULT);
        return 0;
    }
    return 1;
}

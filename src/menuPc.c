#include <string.h>

#include "app/floor/floorEvent.h"
#include "app/menu/itemStorage.h"
#include "app/menu/menuModel.h"
#include "app/menu/menuWindow.h"
#include "app/menu/pcMenu.h"
#include "app/menu/pokemonRoster.h"
#include "app/menu/screenElement.h"
#include "app/model/modelInfo.h"
#include "app/scene/scene.h"

enum {
    PC_MENU_SOUND_VOLUME = 85,
    PC_MENU_SOUND_FADE_IN = 500,
    PC_MENU_SOUND_FADE_OUT = 1000
};
#define PC_MENU_MODEL_EXTRA_WIDTH 10

#define PC_MENU_PI 3.14159265f

typedef struct PcOptionMotion {
    float delay;
    float rate;
    float progress;
    float baseX;
    float slideOffset;
    float settledOffset;
} PcOptionMotion;

PcOptionMotion pcMenuOptionOpened[PC_MENU_OPTION_COUNT] = {
    /* PC_MENU_OPTION_POKEMON_STORAGE */ {0.0f, 50.0f, 0.0f, 42.0f, -300.0f, 0.0f},
    /* PC_MENU_OPTION_ITEM_STORAGE    */ {0.0f, 45.0f, 0.0f, 52.0f, -300.0f, 0.0f},
    /* PC_MENU_OPTION_SAVE            */ {0.0f, 40.0f, 0.0f, 62.0f, -300.0f, 0.0f},
    /* PC_MENU_OPTION_CANCEL          */ {0.0f, 35.0f, 0.0f, 72.0f, -300.0f, 0.0f},
};

PcOptionMotion pcMenuOptionClosed[PC_MENU_OPTION_COUNT] = {
    /* PC_MENU_OPTION_POKEMON_STORAGE */ {0.0f, 50.0f, 0.0f, 58.0f, -300.0f, 0.0f},
    /* PC_MENU_OPTION_ITEM_STORAGE    */ {0.0f, 45.0f, 0.0f, 68.0f, -300.0f, 0.0f},
    /* PC_MENU_OPTION_SAVE            */ {0.0f, 40.0f, 0.0f, 78.0f, -300.0f, 0.0f},
    /* PC_MENU_OPTION_CANCEL          */ {0.0f, 35.0f, 0.0f, 88.0f, -300.0f, 0.0f},
};

float pcMenuOptionLabelPosition[PC_MENU_OPTION_COUNT][2] = {
    /* PC_MENU_OPTION_POKEMON_STORAGE */ {42.0f, 179.0f},
    /* PC_MENU_OPTION_ITEM_STORAGE    */ {52.0f, 234.0f},
    /* PC_MENU_OPTION_SAVE            */ {62.0f, 289.0f},
    /* PC_MENU_OPTION_CANCEL          */ {72.0f, 344.0f},
};

float pcMenuOptionAngle[PC_MENU_OPTION_COUNT] = {
    0.0f, PC_MENU_PI / 2.0f, PC_MENU_PI, PC_MENU_PI * 1.5f,
};

float pcMenuOptionNextAngle[PC_MENU_OPTION_COUNT] = {
    PC_MENU_PI / 2.0f, PC_MENU_PI, PC_MENU_PI * 1.5f, 0.0f,
};

PcMenuModel pcMenuOptionModel;
PcOptionMotion pcMenuOptionClosedWork[PC_MENU_OPTION_COUNT];
PcOptionMotion pcMenuOptionOpenedWork[PC_MENU_OPTION_COUNT];

void pcMenuLoop(void)
{
    PcMenuState state = PC_MENU_STATE_START;
    int running = 1;
    int option;
    int motion;
    u8 available;
    PcOptionMotion *work;
    int j;
    const PcOptionMotion *placements;

    soundPushVolume(PC_MENU_SOUND_VOLUME, PC_MENU_SOUND_FADE_IN, 1, 1);

    do {
        switch (state) {
        case PC_MENU_STATE_START:
            pcMenuPrompt = MESSAGE_ID_PC_WHICH_TO_ACCESS;
            for (j = 0, placements = pcMenuOptionOpened, work = pcMenuOptionOpenedWork;
                j < PC_MENU_OPTION_COUNT; placements++, work++, j++) {
                memcpy(work, placements, sizeof(PcOptionMotion));
            }
            for (j = 0, placements = pcMenuOptionClosed, work = pcMenuOptionClosedWork;
                j < PC_MENU_OPTION_COUNT; placements++, work++, j++) {
                memcpy(work, placements, sizeof(PcOptionMotion));
            }
            pcMenuRequestedOption = PC_MENU_OPTION_POKEMON_STORAGE;
            pcMenuTargetAngle = pcMenuOptionAngle[PC_MENU_OPTION_POKEMON_STORAGE];
            pcMenuCurrentAngle = pcMenuOptionAngle[PC_MENU_OPTION_POKEMON_STORAGE];
            pcMenuShownOption = PC_MENU_OPTION_POKEMON_STORAGE;
            menuWindowOpen(MENU_WINDOW_PC_STORAGE, 0);
            menuWindowOpen(MENU_WINDOW_PC_SUBTITLE, 0);
            menuWindowOpen(MENU_WINDOW_PC_PROMPT, 0);
            menuWindowOpen(MENU_WINDOW_PC_BOX, 0);
            menuWindowOpen(MENU_WINDOW_PC_LIST, 0);
            menuWindowOpen(MENU_WINDOW_PC_PARTY, 0);
            state = PC_MENU_STATE_SELECT;
            break;

        case PC_MENU_STATE_SELECT:
            pcMenuSelectedOption = -1;
            menuWindowOpen(MENU_WINDOW_PC_POKEMON_STORAGE, 0);
            menuWindowOpen(MENU_WINDOW_PC_ITEM_STORAGE, 0);
            menuWindowOpen(MENU_WINDOW_PC_SAVE, 0);
            menuWindowOpen(MENU_WINDOW_PC_CANCEL, 0);
            option = menuWindowOpen(MENU_WINDOW_PC_SELECT, 1);
            if (option >= 0) {
                pcMenuSelectedOption = option;
                switch (option) {
                case PC_MENU_OPTION_POKEMON_STORAGE:
                    menuWindowClose(MENU_WINDOW_PC_POKEMON_STORAGE,
                        MENU_WINDOW_CLOSE_ALL, 1);
                    pcMenuPrompt = MESSAGE_ID_PC_WHICH_TO_ACCESS;
                    for (j = 0, placements = pcMenuOptionOpened, work = pcMenuOptionOpenedWork;
                        j < PC_MENU_OPTION_COUNT; placements++, work++, j++) {
                        memcpy(work, placements, sizeof(PcOptionMotion));
                    }
                    for (j = 0, placements = pcMenuOptionClosed, work = pcMenuOptionClosedWork;
                        j < PC_MENU_OPTION_COUNT; placements++, work++, j++) {
                        memcpy(work, placements, sizeof(PcOptionMotion));
                    }
                    pcMenuRequestedOption = PC_MENU_OPTION_POKEMON_STORAGE;
                    pcMenuTargetAngle = pcMenuOptionAngle[PC_MENU_OPTION_POKEMON_STORAGE];
                    pcMenuCurrentAngle = pcMenuOptionAngle[PC_MENU_OPTION_POKEMON_STORAGE];
                    pcMenuShownOption = PC_MENU_OPTION_POKEMON_STORAGE;
                    state = PC_MENU_STATE_POKEMON_STORAGE;
                    break;

                case PC_MENU_OPTION_ITEM_STORAGE:
                    menuWindowClose(MENU_WINDOW_PC_POKEMON_STORAGE,
                        MENU_WINDOW_CLOSE_ALL, 1);
                    pcMenuPrompt = MESSAGE_ID_PC_WHICH_TO_ACCESS;
                    for (j = 0, placements = pcMenuOptionOpened, work = pcMenuOptionOpenedWork;
                        j < PC_MENU_OPTION_COUNT; placements++, work++, j++) {
                        memcpy(work, placements, sizeof(PcOptionMotion));
                    }
                    for (j = 0, placements = pcMenuOptionClosed, work = pcMenuOptionClosedWork;
                        j < PC_MENU_OPTION_COUNT; placements++, work++, j++) {
                        memcpy(work, placements, sizeof(PcOptionMotion));
                    }
                    pcMenuRequestedOption = PC_MENU_OPTION_POKEMON_STORAGE;
                    pcMenuTargetAngle = pcMenuOptionAngle[PC_MENU_OPTION_POKEMON_STORAGE];
                    pcMenuCurrentAngle = pcMenuOptionAngle[PC_MENU_OPTION_POKEMON_STORAGE];
                    pcMenuShownOption = PC_MENU_OPTION_POKEMON_STORAGE;
                    state = PC_MENU_STATE_ITEM_STORAGE;
                    break;

                case PC_MENU_OPTION_SAVE:
                    menuWindowClose(MENU_WINDOW_PC_POKEMON_STORAGE,
                        MENU_WINDOW_CLOSE_ALL, 1);
                    pcMenuPrompt = MESSAGE_ID_PC_WHICH_TO_ACCESS;
                    for (j = 0, placements = pcMenuOptionOpened, work = pcMenuOptionOpenedWork;
                        j < PC_MENU_OPTION_COUNT; placements++, work++, j++) {
                        memcpy(work, placements, sizeof(PcOptionMotion));
                    }
                    for (j = 0, placements = pcMenuOptionClosed, work = pcMenuOptionClosedWork;
                        j < PC_MENU_OPTION_COUNT; placements++, work++, j++) {
                        memcpy(work, placements, sizeof(PcOptionMotion));
                    }
                    pcMenuRequestedOption = PC_MENU_OPTION_POKEMON_STORAGE;
                    pcMenuTargetAngle = pcMenuOptionAngle[PC_MENU_OPTION_POKEMON_STORAGE];
                    pcMenuCurrentAngle = pcMenuOptionAngle[PC_MENU_OPTION_POKEMON_STORAGE];
                    pcMenuShownOption = PC_MENU_OPTION_POKEMON_STORAGE;
                    state = PC_MENU_STATE_SAVE;
                    break;

                case PC_MENU_OPTION_CANCEL:
                    state = PC_MENU_STATE_END;
                    break;
                }
            } else {
                state = PC_MENU_STATE_END;
            }
            break;

        case PC_MENU_STATE_POKEMON_STORAGE:
            runPokemonRosterScreen();
            state = PC_MENU_STATE_SELECT;
            break;

        case PC_MENU_STATE_ITEM_STORAGE:
            pcMenuPrompt = MESSAGE_ID_PC_WHAT_TO_DO;
            runItemStorageMenu();
            pcMenuPrompt = MESSAGE_ID_PC_WHICH_TO_ACCESS;
            state = PC_MENU_STATE_SELECT;
            break;

        case PC_MENU_STATE_SAVE:
            menuModelInit(&pcMenuOptionModel.model,
                screenElements[SCREEN_ELEMENT_PC_MENU].width +
                    PC_MENU_MODEL_EXTRA_WIDTH,
                screenElements[SCREEN_ELEMENT_PC_MENU].height);
            menuModelSetResource(&pcMenuOptionModel.model, MODEL_ID_PC_MENU);
            getModelMotion(findModelInfo(MODEL_ID_PC_MENU),
                MODEL_MOTION_IDLE, &motion, &available);
            menuModelSetMotion(&pcMenuOptionModel.model, motion);
            state = confirmAndSaveGame();
            menuModelTerm(&pcMenuOptionModel.model);
            break;

        case PC_MENU_STATE_LEAVE:
            state = PC_MENU_STATE_END;
            break;

        case PC_MENU_STATE_END:
            menuWindowClose(MENU_WINDOW_PC_SELECT,
                MENU_WINDOW_CLOSE_ALL, 1);
            running = 0;
            break;
        }

    } while (running);

    soundPopVolume(PC_MENU_SOUND_FADE_OUT, 1, 1);
    endScene();
}

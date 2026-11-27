#ifndef APP_MENU_DBGMENUFIGHTTRAINER_H
#define APP_MENU_DBGMENUFIGHTTRAINER_H

typedef struct MenuWindow MenuWindow;
typedef struct FloorCommandEntry FloorCommandEntry;

#ifdef __cplusplus
extern "C" {
#endif

void dbgMenuFightTrainerSetAvailable(MenuWindow *window, FloorCommandEntry *entry);

#ifdef __cplusplus
}
#endif

#endif

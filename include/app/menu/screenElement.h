#ifndef APP_MENU_SCREENELEMENT_H
#define APP_MENU_SCREENELEMENT_H

#include "pokemon/types.h"

typedef struct MenuWindow MenuWindow;
typedef struct MenuWindowContext MenuWindowContext;

typedef enum ScreenElementId {
    SCREEN_ELEMENT_PC_MENU = 876,
} ScreenElementId;

typedef struct ScreenElement {
    u8 flags;
    u8 alpha;
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    s16 graphicId;
    u16 group;
    u16 style;
    u32 messageId;
    void (*callback)(MenuWindow *window, MenuWindowContext *context);
    s16 nextIdx;
} ScreenElement;

#ifdef __cplusplus
extern "C" {
#endif

extern ScreenElement screenElements[];

#ifdef __cplusplus
}
#endif

#endif

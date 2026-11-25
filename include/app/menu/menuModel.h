#ifndef APP_MENU_MENUMODEL_H
#define APP_MENU_MENUMODEL_H

#include "pokemon/types.h"

typedef struct ModelInstance ModelInstance;
typedef struct ModelRequest ModelRequest;
typedef struct MenuRenderTarget MenuRenderTarget;
typedef struct MenuCamera MenuCamera;
typedef struct MenuQuad MenuQuad;

typedef struct MenuModelParam {
    u8 kind;
    u32 key;
    u32 param;
    u16 anim;
    u8 animArg;
} MenuModelParam;

typedef struct MenuModel {
    u8 state;
    u8 loading;
    MenuModelParam requested;
    MenuModelParam current;
    ModelInstance *model;
    ModelRequest *request;
    s32 width;
    s32 height;
    MenuRenderTarget *renderTarget;
    MenuCamera *camera;
    MenuQuad *quad[3];
} MenuModel;

#ifdef __cplusplus
extern "C" {
#endif

int menuModelInit(MenuModel *model, s16 width, s16 height);
int menuModelSetResource(MenuModel *model, u32 modelId);
int menuModelSetMotion(MenuModel *model, int motion);
int menuModelTerm(MenuModel *model);

#ifdef __cplusplus
}
#endif

#endif

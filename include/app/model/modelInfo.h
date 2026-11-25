#ifndef APP_MODEL_MODELINFO_H
#define APP_MODEL_MODELINFO_H

#include "pokemon/types.h"

typedef struct ModelInfo ModelInfo;

typedef enum ModelId {
    MODEL_ID_PC_MENU = 16188416,
} ModelId;

typedef enum ModelMotion {
    MODEL_MOTION_IDLE = 1,
} ModelMotion;

#ifdef __cplusplus
extern "C" {
#endif

ModelInfo *findModelInfo(u32 modelId);
void getModelMotion(const ModelInfo *info, u8 motion, int *value,
                    u8 *available);

#ifdef __cplusplus
}
#endif

#endif

#ifndef GSAPI_GSMODEL_GSMODEL_H
#define GSAPI_GSMODEL_GSMODEL_H

#include <dolphin/types.h>

typedef struct GSmodel GSmodel;

/*
 * Texture animation state of a model, held in the model flag word. The model
 * update advances the texture animation while it runs; starting the animation
 * is ignored on models that are not set up with texture animation data.
 */
typedef enum GSmodelFlag {
    GSMODEL_FLAG_TEXANIM_ENABLED = 0x00000004,
    GSMODEL_FLAG_TEXANIM_PRESENT = 0x00000008, /* the model has texture animation data */
    GSMODEL_FLAG_TEXANIM_RUNNING = 0x00000040, /* the model update advances the animation */
    GSMODEL_FLAG_TEXANIM_LINKED = 0x00002000,
    GSMODEL_FLAG_TEXANIM_FINISHED = 0x00008000, /* the animation ran to the end of its range */
} GSmodelFlag;

struct GSmodel {
    u32 flags;
};

void GSmodelLinkTexAnimToAnim(GSmodel *model, u8 link);
u8 GSmodelTexAnimIsPresent(GSmodel *model);
u8 GSmodelTexAnimIsEnabled(GSmodel *model);
u8 GSmodelTexAnimIsFinished(GSmodel *model);
void GSmodelStopTexAnimation(GSmodel *model);
void GSmodelStartTexAnimation(GSmodel *model);

#endif

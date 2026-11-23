#ifndef GSAPI_GSMODEL_GSMODEL_H
#define GSAPI_GSMODEL_GSMODEL_H

#include <dolphin/types.h>

typedef struct GSmodel GSmodel;

/*
 * Texture animation state of a model, held in the model flag word. The model
 * update advances the texture animation while it runs; starting the animation
 * is ignored on models that are not set up with texture animation data.
 */
#define GSMODEL_FLAG_TEXANIM_PRESENT  0x00000008 /* the model has texture animation data */
#define GSMODEL_FLAG_TEXANIM_RUNNING  0x00000040 /* the model update advances the animation */
#define GSMODEL_FLAG_TEXANIM_FINISHED 0x00008000 /* the animation ran to the end of its range */

struct GSmodel {
    u32 flags;
};

void GSmodelStopTexAnimation(GSmodel *model);
void GSmodelStartTexAnimation(GSmodel *model);

#endif

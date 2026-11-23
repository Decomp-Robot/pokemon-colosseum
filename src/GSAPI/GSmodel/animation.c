/*
 * Start and stop of a model's texture animation. The model update advances the
 * animation while it runs, and marks it finished when it runs to the end of its
 * range; starting the animation clears that finished state.
 */
#include "GSAPI/GSmodel/GSmodel.h"

void GSmodelStopTexAnimation(GSmodel *model)
{
    model->flags &= ~GSMODEL_FLAG_TEXANIM_RUNNING;
}

void GSmodelStartTexAnimation(GSmodel *model)
{
    if ((model->flags & GSMODEL_FLAG_TEXANIM_PRESENT) == 0) {
        return;
    }

    model->flags |= GSMODEL_FLAG_TEXANIM_RUNNING;
    model->flags &= ~GSMODEL_FLAG_TEXANIM_FINISHED;
}

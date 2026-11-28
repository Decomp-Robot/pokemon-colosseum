#include "GSAPI/GSmodel/GSmodel.h"

void GSmodelLinkTexAnimToAnim(GSmodel *model, u8 link)
{
    if (link != 0) {
        model->flags |= GSMODEL_FLAG_TEXANIM_LINKED;
    } else {
        model->flags &= ~GSMODEL_FLAG_TEXANIM_LINKED;
    }
}

u8 GSmodelTexAnimIsPresent(GSmodel *model)
{
    return (model->flags & GSMODEL_FLAG_TEXANIM_PRESENT) != 0;
}

u8 GSmodelTexAnimIsEnabled(GSmodel *model)
{
    return (model->flags & GSMODEL_FLAG_TEXANIM_ENABLED) != 0;
}

u8 GSmodelTexAnimIsFinished(GSmodel *model)
{
    return (model->flags & GSMODEL_FLAG_TEXANIM_FINISHED) != 0;
}

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

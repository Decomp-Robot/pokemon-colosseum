#ifndef SYSDOLPHIN_BASELIB_SPLINE_H
#define SYSDOLPHIN_BASELIB_SPLINE_H

#include <dolphin/mtx.h>

typedef struct _HSD_Spline HSD_Spline;

void HSD_SplineEvaluate(Vec* pos, HSD_Spline* spline, f32 t);

#endif

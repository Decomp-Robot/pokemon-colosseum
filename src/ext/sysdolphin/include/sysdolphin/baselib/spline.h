#ifndef SYSDOLPHIN_BASELIB_SPLINE_H
#define SYSDOLPHIN_BASELIB_SPLINE_H

#include <dolphin/mtx.h>

typedef struct _HSD_Spline HSD_Spline;

f32 HSD_SplineEval(f32 ratio, f32 time, f32 p0, f32 p1, f32 d0, f32 d1);
void HSD_SplineEvaluate(Vec* pos, HSD_Spline* spline, f32 t);

#endif

#ifndef SYSDOLPHIN_BASELIB_DEBUG_H
#define SYSDOLPHIN_BASELIB_DEBUG_H

/* Reports a failed assertion and then panics. Assertions are always active. */
void HSD_AssertFail(const char* file, int line, const char* cond);
void HSD_Panic(const char* file, int line, const char* msg);

#define HSD_ASSERT(line, cond)                                                \
    ((cond) ? ((void) 0) : HSD_AssertFail(__FILE__, line, #cond))

#define HSD_ERR(line, msg) HSD_Panic(__FILE__, line, msg)

#endif

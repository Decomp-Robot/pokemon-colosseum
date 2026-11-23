#include <dolphin/os.h>

void MWEnterCriticalSection(BOOL *section)
{
    *section = OSDisableInterrupts();
}

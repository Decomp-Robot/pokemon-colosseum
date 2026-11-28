#include <libc/string.h>

#include "app/menu/debugMenu.h"

#define DBG_MENU_RELIVE_RECORD_SIZE 24

int dbgMenuReliveItemCreate(DbgMenuItem *item)
{
    if (item->mode == DBG_MENU_ITEM_MODE_CREATE) {
        void *record = dbgMenuAllocateRecord(item, DBG_MENU_RELIVE_RECORD_SIZE);
        if (record != 0) {
            memcpy(record, item->args[0], DBG_MENU_RELIVE_RECORD_SIZE);
        }
    }
    return 0;
}

/*
 * Floor-stack state helpers.
 *
 * Both helpers look up the floor data for the room currently being played,
 * then fetch the corresponding push-state or push-data block from that data.
 */
extern void *zz_801170A4_(void);
extern void *zz_80116EC8_(void *floorData);
extern void *zz_80116EB0_(void *floorData);

void *getPushState__14FloorStackListFv(void)
{
    return zz_80116EC8_(zz_801170A4_());
}

void *reallocPushData__14FloorStackListFv(void)
{
    return zz_80116EB0_(zz_801170A4_());
}

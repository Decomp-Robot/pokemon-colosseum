#include <stdlib.h>

#define SWAP_BYTES(firstElement, secondElement, elementSize) \
    do { \
        char *first = (firstElement) - 1; \
        char *second = (secondElement) - 1; \
        size_t width = (elementSize) + 1; \
        while (--width) { \
            int temp = *++second; \
            *second = *++first; \
            *first = temp; \
        } \
    } while (0)

void qsort(void *base, size_t count, size_t elementSize,
           int (*compare)(const void *, const void *)) {
    size_t parent;
    size_t heapSize;
    size_t child;
    char *basePtr = (char *)base;
    char *parentPtr;
    char *lastPtr;
    char *siftParent;
    char *childPtr;

    if (count < 2) {
        return;
    }

    parent = (count >> 1) + 1;
    heapSize = count;
    parentPtr = basePtr + elementSize * (parent - 1);
    lastPtr = basePtr + elementSize * (count - 1);

    for (;;) {
        if (parent > 1) {
            parentPtr -= elementSize;
            parent--;
        } else {
            SWAP_BYTES(lastPtr, parentPtr, elementSize);
            if (--heapSize == 1) {
                return;
            }
            lastPtr -= elementSize;
        }

        child = parent;
        childPtr = basePtr + elementSize * (child - 1);
        while (child * 2 <= heapSize) {
            child *= 2;
            siftParent = childPtr;
            childPtr = basePtr + elementSize * (child - 1);
            if (child < heapSize) {
                char *rightPtr = childPtr + elementSize;
                if (compare(childPtr, rightPtr) < 0) {
                    childPtr = rightPtr;
                    child++;
                }
            }
            if (compare(siftParent, childPtr) >= 0) {
                break;
            }
            SWAP_BYTES(childPtr, siftParent, elementSize);
        }
    }
}

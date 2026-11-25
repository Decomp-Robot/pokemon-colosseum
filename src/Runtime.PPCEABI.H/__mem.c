#include <string.h>

void __fill_mem(void* dst, int val, size_t n);

__declspec(section ".init") void* memset(void* dest, int val, size_t count)
{
    __fill_mem(dest, val, count);
    return dest;
}

__declspec(section ".init") void __fill_mem(void* dst, int val, size_t n)
{
    unsigned char* byteDst = (unsigned char*)dst - 1;
    unsigned long* wordDst;
    unsigned long i;
    unsigned long v = (unsigned char)val;

    if (n >= 32) {
        i = (~(unsigned long)byteDst) & 3;

        if (i) {
            n -= i;

            do
                *++byteDst = v;
            while (--i);
        }

        if (v)
            v |= v << 24 | v << 16 | v << 8;

        wordDst = (unsigned long*)(byteDst + 1) - 1;

        i = n >> 5;

        if (i)
            do {
                *++wordDst = v;
                *++wordDst = v;
                *++wordDst = v;
                *++wordDst = v;
                *++wordDst = v;
                *++wordDst = v;
                *++wordDst = v;
                *++wordDst = v;
            } while (--i);

        i = (n & 31) >> 2;

        if (i)
            do
                *++wordDst = v;
            while (--i);

        byteDst = (unsigned char*)(wordDst + 1) - 1;

        n &= 3;
    }

    if (n)
        do
            *++byteDst = v;
        while (--n);
}

__declspec(section ".init") void* memcpy(void* dst, const void* src, size_t n)
{
    const unsigned char* p;
    unsigned char* q;

    if (src >= dst) {
        p = (const unsigned char*)src - 1;
        q = (unsigned char*)dst - 1;
        ++n;
        while (--n)
            *++q = *++p;
    } else {
        p = (const unsigned char*)src + n;
        q = (unsigned char*)dst + n;
        ++n;
        while (--n)
            *--q = *--p;
    }

    return dst;
}

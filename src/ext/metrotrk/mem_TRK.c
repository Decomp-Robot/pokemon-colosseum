void TRK_fill_mem(void* dest, int val, unsigned int length);

__declspec(section ".init") void* TRK_memset(void* dest, int val, unsigned int length) {
    TRK_fill_mem(dest, val, length);
    return dest;
}

__declspec(section ".init") void* TRK_memcpy(void* dest, const void* src, unsigned int length) {
    const unsigned char* s = (const unsigned char*)src - 1;
    unsigned char* d = (unsigned char*)dest - 1;
    unsigned int count = length + 1;

    while (--count) {
        *++d = *++s;
    }

    return dest;
}

#ifndef _MSL_C_MEM_H_
#define _MSL_C_MEM_H_

#include <stddef.h>

#define WORD_COPY_THRESHOLD 32
#define WORD_ALIGNMENT_MASK (sizeof(unsigned int) - 1)

void __copy_longs_unaligned(void * dst, const void * src, size_t n);
void __copy_longs_rev_unaligned(void * dst, const void * src, size_t n);
void __copy_longs_aligned(void * dst, const void * src, size_t n);
void __copy_longs_rev_aligned(void * dst, const void * src, size_t n);

#endif

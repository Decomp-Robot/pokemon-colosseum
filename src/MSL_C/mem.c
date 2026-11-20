/*
 * Memory primitives.
 *
 * memcmp, memchr and __memrchr walk the region a byte at a time.
 *
 * memmove copies a region that may overlap itself, so the copy direction
 * decides whether a source byte is still intact when it is read: once the
 * destination lies above the source the copy has to run backwards.  Regions of
 * 32 bytes or more go to the word-at-a-time helpers, which provide a separate
 * entry point for a source and destination whose alignment offsets differ
 * (unaligned) and for the aligned case.
 */
#include <string.h>
#include "__mem.h"

int memcmp(const void * s1, const void * s2, size_t n)
{
	const unsigned char * p1 = (const unsigned char *) s1 - 1;
	const unsigned char * p2 = (const unsigned char *) s2 - 1;

	++n;
	while (--n)
		if (*++p1 != *++p2)
			return (*p1 < *p2) ? -1 : 1;
	return 0;
}

void * __memrchr(const void * s, int c, size_t n)
{
	unsigned char ch = (unsigned char) c;
	const unsigned char * p = (const unsigned char *) s + n;

	++n;
	while (--n)
		if (*--p == ch)
			return (void *) p;
	return 0;
}

void * memchr(const void * s, int c, size_t n)
{
	unsigned char ch = (unsigned char) c;
	const unsigned char * p = (const unsigned char *) s - 1;

	++n;
	while (--n)
		if (*++p == ch)
			return (void *) p;
	return 0;
}

void * memmove(void * dst, const void * src, size_t n)
{
	const unsigned char * s;
	unsigned char * d;
	int backward = dst > src;

	if (n >= WORD_COPY_THRESHOLD) {
		if (((unsigned int) dst ^ (unsigned int) src) & WORD_ALIGNMENT_MASK) {
			if (!backward)
				__copy_longs_unaligned(dst, src, n);
			else
				__copy_longs_rev_unaligned(dst, src, n);
		} else {
			if (!backward)
				__copy_longs_aligned(dst, src, n);
			else
				__copy_longs_rev_aligned(dst, src, n);
		}
		return dst;
	}

	s = (const unsigned char *) src;
	d = (unsigned char *) dst;
	if (!backward) {
		--s;
		--d;
		++n;
		while (--n)
			*++d = *++s;
	} else {
		s += n;
		d += n;
		++n;
		while (--n)
			*--d = *--s;
	}
	return dst;
}

#include <string.h>
#include "__mem.h"

void __copy_longs_rev_unaligned(void * dst, const void * src, size_t n)
{
	unsigned int lead;
	size_t words;
	unsigned int off;
	unsigned int shift;
	unsigned int rshift;
	unsigned int w;
	unsigned int next;
	const unsigned char * s = (const unsigned char *) src + n;
	unsigned char * d = (unsigned char *) dst + n;

	if ((lead = (unsigned int) d & WORD_ALIGNMENT_MASK) != 0) {
		n -= lead;
		do
			*--d = *--s;
		while (--lead);
	}
	off = (unsigned int) s & WORD_ALIGNMENT_MASK;
	shift = off << 3;
	rshift = 32 - shift;
	words = n >> 3;
	s += 4 - off;
	s -= 4;
	w = *(const unsigned int *) s;
	do {
		next = *((const unsigned int *) s - 1);
		*((unsigned int *) d - 1) = (next << shift) | (w >> rshift);
		s -= 8;
		w = *(const unsigned int *) s;
		*((unsigned int *) d - 2) = (w << shift) | (next >> rshift);
		d -= 8;
	} while (--words);
	if ((n & 4) != 0) {
		s -= 4;
		next = *(const unsigned int *) s;
		*((unsigned int *) d - 1) = (next << shift) | (w >> rshift);
		d -= 4;
	}
	if ((n &= 3) != 0) {
		s += off;
		do
			*--d = *--s;
		while (--n);
	}
}

void __copy_longs_unaligned(void * dst, const void * src, size_t n)
{
	unsigned int * dw;
	size_t words;
	unsigned char * d = (unsigned char *) dst;
	const unsigned char * s = (const unsigned char *) src;
	unsigned int lead;
	unsigned int off;
	unsigned int shift;
	unsigned int rshift;
	unsigned int w;
	unsigned int next;
	const unsigned char * ts;

	s -= 1;
	d -= 1;
	if ((lead = -(unsigned int) dst & WORD_ALIGNMENT_MASK) != 0) {
		n -= lead;
		do
			*++d = *++s;
		while (--lead);
	}
	dw = (unsigned int *) (d + 1) - 1;
	off = (unsigned int) (s + 1) & WORD_ALIGNMENT_MASK;
	words = n >> 3;
	s -= off;
	shift = off << 3;
	s += 1;
	w = *(const unsigned int *) s;
	rshift = 32 - shift;
	do {
		next = *((const unsigned int *) s + 1);
		*++dw = (w << shift) | (next >> rshift);
		s += 8;
		w = *(const unsigned int *) s;
		*++dw = (next << shift) | (w >> rshift);
	} while (--words);
	if ((n & 4) != 0) {
		s += 4;
		next = *(const unsigned int *) s;
		*++dw = (w << shift) | (next >> rshift);
	}
	ts = s + 3;
	d = (unsigned char *) dw + 3;
	if ((n &= 3) != 0) {
		s = ts - (4 - off);
		do
			*++d = *++s;
		while (--n);
	}
}

void __copy_longs_rev_aligned(void * dst, const void * src, size_t n)
{
	unsigned int lead;
	size_t blocks;
	size_t words;
	const unsigned char * s = (const unsigned char *) src + n;
	unsigned char * d = (unsigned char *) dst + n;

	if ((lead = (unsigned int) d & WORD_ALIGNMENT_MASK) != 0) {
		n -= lead;
		do
			*--d = *--s;
		while (--lead);
	}
	if ((blocks = n >> 5) != 0) {
		do {
			*((unsigned int *) d - 1) = *((const unsigned int *) s - 1);
			*((unsigned int *) d - 2) = *((const unsigned int *) s - 2);
			*((unsigned int *) d - 3) = *((const unsigned int *) s - 3);
			*((unsigned int *) d - 4) = *((const unsigned int *) s - 4);
			*((unsigned int *) d - 5) = *((const unsigned int *) s - 5);
			*((unsigned int *) d - 6) = *((const unsigned int *) s - 6);
			*((unsigned int *) d - 7) = *((const unsigned int *) s - 7);
			*((unsigned int *) d - 8) = *((const unsigned int *) s - 8);
			d -= 32;
			s -= 32;
		} while (--blocks);
	}
	if ((words = (n >> 2) & 7) != 0) {
		do {
			*((unsigned int *) d - 1) = *((const unsigned int *) s - 1);
			d -= 4;
			s -= 4;
		} while (--words);
	}
	if ((n &= 3) != 0) {
		do
			*--d = *--s;
		while (--n);
	}
}

void __copy_longs_aligned(void * dst, const void * src, size_t n)
{
	const unsigned char * s = (const unsigned char *) src - 1;
	unsigned char * d = (unsigned char *) dst - 1;
	unsigned int lead;
	size_t blocks;
	size_t words;
	unsigned int * dw;
	const unsigned int * sw;
	const unsigned char * ts;

	if ((lead = -(unsigned int) dst & WORD_ALIGNMENT_MASK) != 0) {
		n -= lead;
		do
			*++d = *++s;
		while (--lead);
	}
	sw = (const unsigned int *) (s + 1) - 1;
	dw = (unsigned int *) (d + 1) - 1;
	if ((blocks = n >> 5) != 0) {
		do {
			*++dw = *++sw;
			*++dw = *++sw;
			*++dw = *++sw;
			*++dw = *++sw;
			*++dw = *++sw;
			*++dw = *++sw;
			*++dw = *++sw;
			*++dw = *++sw;
		} while (--blocks);
	}
	if ((words = (n >> 2) & 7) != 0) {
		do
			*++dw = *++sw;
		while (--words);
	}
	ts = (const unsigned char *) sw + 3;
	d = (unsigned char *) dw + 3;
	if ((n &= 3) != 0) {
		do
			*++d = *++ts;
		while (--n);
	}
}

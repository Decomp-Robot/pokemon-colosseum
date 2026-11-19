/*
 * Character string primitives.
 *
 * strcpy and strcmp use 32-bit loads only when both pointers have the same
 * offset within a word; otherwise they stay byte-wise.  When the offsets
 * agree, leading bytes are taken one at a time to reach the next word
 * boundary, then the copy or comparison proceeds a word at a time.  A clear
 * MAY_HAVE_NUL result proves that word has no NUL byte, so the word loops may
 * advance; a nonzero result is only a possible hit and the byte path decides
 * whether a terminator is really present.
 */
#include <string.h>

#define BYTE_ONES 0x01010101U
#define BYTE_HIGH_BITS 0x80808080U
/* A clear result proves the word has no NUL byte; a set result does not prove it has one. */
#define MAY_HAVE_NUL(w) ((((w) - BYTE_ONES) & BYTE_HIGH_BITS) != 0)
#define WORD_ALIGNMENT_MASK (sizeof(unsigned int) - 1)

char * strchr(const char * str, int chr)
{
	const unsigned char * p = (const unsigned char *) str - 1;
	unsigned char target = chr;
	unsigned int byte;

	while ((byte = *++p) != 0) {
		if (byte == target)
			return (char *) p;
	}
	return target ? 0 : (char *) p;
}

int strncmp(const char * s1, const char * s2, size_t n)
{
	const unsigned char * p1 = (const unsigned char *) s1 - 1;
	const unsigned char * p2 = (const unsigned char *) s2 - 1;
	unsigned int c1, c2;

	++n;
	while (--n) {
		c1 = *++p1;
		c2 = *++p2;
		if (c1 != c2)
			return (int) c1 - (int) c2;
		if (c1 == 0)
			break;
	}
	return 0;
}

int strcmp(const char * str1, const char * str2)
{
	const unsigned char * p1 = (const unsigned char *) str1;
	const unsigned char * p2 = (const unsigned char *) str2;
	int c1 = *p1;
	unsigned int word_offset;
	int diff = c1 - *p2;

	if (diff != 0)
		return diff;

	if ((word_offset = (unsigned int) p1 & WORD_ALIGNMENT_MASK) == ((unsigned int) p2 & WORD_ALIGNMENT_MASK)) {
		const unsigned int * word1;
		const unsigned int * word2;
		unsigned int w1, w2;

		if (word_offset != 0) {
			unsigned int prefix_bytes;

			if (c1 == 0U)
				return 0;
			for (prefix_bytes = WORD_ALIGNMENT_MASK - word_offset; prefix_bytes != 0; --prefix_bytes) {
				c1 = *++p1;
				diff = c1 - *++p2;
				if (diff != 0)
					return diff;
				if (c1 == 0U)
					return 0;
			}
			++p1;
			++p2;
		}

		word1 = (const unsigned int *) p1;
		word2 = (const unsigned int *) p2;
		w1 = *word1;
		w2 = *word2;
		if (!MAY_HAVE_NUL(w1)) {
			while (w1 == w2) {
				w1 = *++word1;
				w2 = *++word2;
				if (MAY_HAVE_NUL(w1))
					goto bytes;
			}
			return w1 > w2 ? 1 : -1;
		}

bytes:
		p1 = (const unsigned char *) word1;
		p2 = (const unsigned char *) word2;
		c1 = *p1;
		diff = c1 - *p2;
		if (diff != 0)
			return diff;
	}

	if (c1 == 0U)
		return 0;
	do {
		c1 = *++p1;
		diff = c1 - *++p2;
		if (diff != 0)
			return diff;
	} while (c1 != 0U);
	return 0;
}

char * strncpy(char * dest, const char * src, size_t n)
{
	const unsigned char * s = (const unsigned char *) src - 1;
	unsigned char * d = (unsigned char *) dest - 1;

	++n;
	while (--n) {
		if ((*++d = *++s) == 0) {
			while (--n)
				*++d = 0;
			break;
		}
	}
	return dest;
}

char * strcpy(char * dest, const char * src)
{
	unsigned char * d = (unsigned char *) dest;
	const unsigned char * s = (const unsigned char *) src;
	unsigned int word_offset;

	if (((unsigned int) dest & WORD_ALIGNMENT_MASK) == (word_offset = (unsigned int) src & WORD_ALIGNMENT_MASK)) {
		const unsigned int * sw;
		unsigned int w;

		if (word_offset != 0) {
			unsigned int prefix_bytes;

			if ((*d = *s) == 0)
				return dest;
			for (prefix_bytes = WORD_ALIGNMENT_MASK - word_offset; prefix_bytes != 0; --prefix_bytes) {
				if ((*++d = *++s) == 0)
					return dest;
			}
			++d;
			++s;
		}

		sw = (const unsigned int *) s;
		w = *sw;
		if (!MAY_HAVE_NUL(w)) {
			unsigned int * dw = (unsigned int *) d - 1;

			do {
				*++dw = w;
				w = *++sw;
			} while (!MAY_HAVE_NUL(w));
			d = (unsigned char *) (dw + 1);
			s = (const unsigned char *) sw;
		}
	}

	if ((*d = *s) != 0) {
		while ((*++d = *++s) != 0)
			;
	}
	return dest;
}

size_t strlen(const char * str)
{
	const unsigned char * p = (const unsigned char *) str - 1;
	size_t len = -1;

	do {
		++len;
	} while (*++p);
	return len;
}

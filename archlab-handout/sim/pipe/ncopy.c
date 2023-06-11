#include <stdio.h>

typedef word_t word_t;

word_t src[8], dst[8];

/* $begin ncopy */
/*
 * ncopy - copy src to dst, returning number of positive ints
 * contained in src array.
 */
#define T 8
word_t ncopy(word_t *src, word_t *dst, word_t len)
{
    word_t count = 0;
    word_t val;

    
	while(len>T)
	{
		val = *src;
        *dst = val;

        val = *(src+1);
        *(dst+1) = val;

        val = *(src+2);
        *(dst+2) = val;

        val = *(src+3);
        *(dst+3) = val;
		src+=T;
		dst+=T;
        len-=T;
	}

	while(len>0)
	{
		val = *src++;
        *dst++ = val;
        len--;
	}

    return count;
}
/* $end ncopy */

int main()
{
    word_t i, count;

    for (i=0; i<8; i++)
	src[i]= i+1;
    count = ncopy(src, dst, 8);
    printf ("count=%d\n", count);
    exit(0);
}



/*
 * $Id$
 *
 * Copyright (c) 2001 Nicola Bonelli <bonelli@blackhats.it>
 *
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. 2.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "input_wordlist.h"


char buf[80];

/*
 * MIT Bitcount
 * 
 * Consider a 3 bit number as being 4a+2b+c if we shift it right 1 bit, we have
 * 2a+b subtracting this from the original gives 2a+b+c if we shift the
 * original 2 bits right we get a and so with another subtraction we have
 * a+b+c which is the number of bits in the original number.
 * 
 * Suitable masking  allows the sums of  the octal digits  in a 32 bit  number
 * to appear in  each octal digit.  This  isn't much help  unless we can get
 * all of them summed together.   This can be done by modulo  arithmetic (sum
 * the digits in a number by  molulo the base of the number minus  one) the
 * old "casting out nines" trick  they taught  in school before  calculators
 * were  invented.  Now, using mod 7 wont help us, because our number will
 * very likely have more than 7 bits set.   So add  the octal digits
 * together to  get base64 digits,  and use modulo 63.   (Those of you  with
 * 64  bit machines need  to add 3  octal digits together to get base512
 * digits, and use mod 511.)
 * 
 * This is HACKMEM 169, as used in X11 sources. Source: MIT AI Lab memo, late
 * 1970's.
 */

#ifdef __GNUC__
__inline
#endif
    int
mit_bitcount (unsigned int n)
{
    /* works for 32-bit numbers only */
    register unsigned int tmp;

    tmp = n - ((n >> 1) & 033333333333) - ((n >> 2) & 011111111111);
    return ((tmp + (tmp >> 3)) & 030707070707) % 63;
}

#ifdef __GNUC__
__inline
#endif
    int
metric (a, b)
     char *a;
     char *b;
{
    register unsigned int ret;
    register int *i = (int *) buf;
    register char *r;

    memset (buf, 0, 80);

    ret = 0;

    r = buf;
    while ((*r++ = *a))
	a++;

    r = buf;
    while (*r++ ^= *b)
	b++;

    for (; *i != 0; i++) {
	ret += mit_bitcount (*i);
    }

    return ret;

}


int line;


void
gsort (char **table, int len)
{
    register int distance;
    register int newdist;
    char *tmp;
    int i;
    int j;

    distance = 0;
    newdist = 0;

    for (i = 0; i < line - 2; i++) {

	fprintf (stderr, "step # %d/%d (%d%%)\r", i, line - 2, i * 100 / line);

	distance = metric (table[i], table[i + 1]);

	for (j = i + 2; j < line; j++) {

	    newdist = metric (table[i], table[j]);

	    if (distance > newdist) {
		tmp = table[i + 1];
		table[i + 1] = table[j];
		table[j] = tmp;
		distance = newdist;
	    }

	}

    }

    return;
}


int
main (int argc, char **argv)
{
    char **table;
    int i;
    fi  fd;

    if (argc == 1) {
	printf ("%s dictionary\n", argv[0]);
	exit (0);
    }

    if ((fd = open_fi (argv[1], O_RDONLY)) == -1)
	exit (1);

    line = info_fi (fd, INFO_LINE);

    table = (char **) calloc (line, sizeof (char *));

    fprintf (stderr, "loading table: ");
    fflush (stderr);

    for (i = 0; i < line; i++) {
	table[i] = (char *) strdup (fgets_fi (buf, 79, fd));
    }

    fprintf (stderr, "done\n");
    fprintf (stderr, "sorting a gray-dictionary...\n");

    gsort (table, line);
    fprintf (stderr, "done\n");

    for (i = 0; i < line; i++)
	fprintf (stdout, "%s\n", table[i]);

    exit (1);

}

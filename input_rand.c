/*
 * All rights reserved.
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


#include <stdlib.h>
#include "global.h"
#include "thash.h"

#ifndef STANDARD
#include "standard.h"
#endif
#ifndef RAND
#include "rand.h"
#endif

char enctab[64] = {		/* radix64 encoding table */
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};


static char radix_vector[1365];
static randctx ctx;
static ub1 c_vect;
static char *cc = (char *) ctx.randrsl;

void
b3b4_radix(char *a, char *b)
{
	int i;

	for (i = 0; i < 85; i++) {
		b[(i << 2)] = ((a[(i << 1) + i])) & 0x3f;
		b[(i << 2) + 1] = ((a[(i << 1) + i + 1] << 2) | (a[(i << 1) + i] >> 6)) & 0x3f;
		b[(i << 2) + 2] = ((a[(i << 1) + i + 2] << 4) | (a[(i << 1) + i + 1] >> 4)) & 0x3f;
		b[(i << 2) + 3] = ((a[(i << 1) + i + 2] >> 2)) & 0x3f;
	}

}

int
openr(char *n)
{
	int i;

	drv.w_read = 0;
	c_vect = 0;
	for (i = 0; i < 256; ++i)
		ctx.randrsl[i] = (ub4) 0;
	randinit(&ctx, drv.seed, TRUE);

	return 0;
}


int
resetr()
{
	int i;

	drv.w_read = 0;
	c_vect = 0;
	for (i = 0; i < 256; ++i)
		ctx.randrsl[i] = (ub4) 0;
	randinit(&ctx, drv.seed, TRUE);

	return 0;
}


char *
readr(char *b, int null)
{
	int i;

	if (drv.w_read == drv.w_max)
		return NULL;

	for (i = 0; i < drv.w_len; i++, c_vect++) {

		if (!(c_vect & 0x03ff)) {
			isaac(&ctx);
			c_vect = 0;
		}
		b[i] = cc[c_vect];
	}

	b[drv.w_len] = 0;
	drv.w_read++;

	return b;
}


char *
readr64(char *b, int null)
{
	int i;

	if (drv.w_read == drv.w_max)
		return NULL;

	for (i = 0; i < drv.w_len; i++, c_vect++) {

		if (!(c_vect & 0x03ff)) {
			isaac(&ctx);
			b3b4_radix(cc, radix_vector);
			c_vect = 0;
		}
		b[i] = enctab[(int) radix_vector[c_vect]];
	}

	b[drv.w_len] = 0;
	drv.w_read++;

	return b;
}


int
closer()
{
	return 0;
}

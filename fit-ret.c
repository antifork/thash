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
#include <limits.h>
#include <math.h>

#include "config.h"
#include "collision.h"

#include "local.h"
#include "global.h"
#include "prototype.h"
#include "macro.h"

double expect;
double cross;
double mean;
double chi2;
double var;

unsigned int min;
unsigned int max;
unsigned int sum;
unsigned int col;
unsigned int fc;

#define FNV_32_PRIME 		16777619
#define FNV_32_INIT 		(0x811c9dc5)
#define MAX_32BIT 		((unsigned int)0xffffffff) /* largest 32 bit unsigned value */
#define RETRY_LEVEL 		((MAX_32BIT / tablen) * tablen)

void
hash_retry ()
{

    unsigned long h;
    int *table;
    int i;
    int j;

    min = UINT_MAX;
    i = 0;
    j = 0;


    if ( drv.open (media) == -1)
        FATAL ("open interface error!");

    drv.reset();

    table = (int *) calloc (tablen, sizeof (int));

    while ( drv.read (buf, 79) != NULL) {

        h = HASH(buf);

	while (h > RETRY_LEVEL) {
    	h = (h * FNV_32_PRIME) + FNV_32_INIT;
	}
	h %= tablen;

	if (table[h])
	    col++;

	if ((fc == 0) && table[h])
	    fc = j;

	table[h]++;

	j++;

    }

    drv.close ();

    for (i = 0; i < tablen; i++) {
	min = MIN (min, table[i]);
	max = MAX (max, table[i]);
	sum += table[i];
    }

    mean = (double) sum / tablen;

    for (i = 0; i < tablen; i++) {
	var += (double) (table[i] - mean) * (table[i] - mean);
	chi2 += pow ((table[i] - mean), 2) / mean;
	cross += table[i] * log (table[i] + 1);
    }

    var /= tablen;
    expect = (double) col / tablen;
    cross /= sum;

    free (table);

    PUTS ("total collision     : %d\n", col);
    PUTS ("first collision     : %d\n", fc);
    PUTS ("mean collision      : %f\n", expect);
    PUTS ("mean                : %f\n", mean);
    PUTS ("standard dev.       : %f\n", sqrt (var));
    PUTS ("min                 : %d\n", min);
    PUTS ("max                 : %d\n", max);
    PUTS ("cross               : %f\n", cross);
    PUTS ("expected            : %lu\n", tablen);
    PUTS ("chi2                : %f\n", chi2);

    return;
}

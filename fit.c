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

void
hash_fitest()
{

	unsigned long h;
	int *t;
	int i;
	int j;

	min = UINT_MAX;
	i = 0;
	j = 0;

	if (drv.open(media) == -1)
		FATAL("open interface error!");

	drv.reset();

	t = (int *) calloc(tablen, sizeof(int));

	while (drv.read(buf, 79) != NULL) {

		h = hash.drv(buf);

		if (t[h])
			col++;

		if ((fc == 0) && t[h])
			fc = j;

		t[h]++;

		j++;

	}

	drv.close();

	for (i = 0; i < tablen; i++) {
		min = MIN(min, t[i]);
		max = MAX(max, t[i]);
		sum += t[i];
	}

	mean = (double) sum / tablen;

	for (i = 0; i < tablen; i++) {
		var += (double) (t[i] - mean) * (t[i] - mean);
		chi2 += pow((t[i] - mean), 2) / mean;
		cross += t[i] * log(t[i] + 1);
	}

	var /= tablen;
	expect = (double) col / tablen;
	cross /= sum;

	free(t);

	PUTS("total collision     : %d\n", col);
	PUTS("first collision     : %d\n", fc);
	PUTS("mean collision      : %f\n", expect);
	PUTS("mean                : %f\n", mean);
	PUTS("standard dev.       : %f\n", sqrt(var));
	PUTS("min                 : %d\n", min);
	PUTS("max                 : %d\n", max);
	PUTS("cross               : %f\n", cross);
	PUTS("expected            : %lu\n", tablen);
	PUTS("chi2                : %f\n", chi2);

	return;
}

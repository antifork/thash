/*
 * $Id$
 *
 * Copyright (c) 2001 Nicola Bonelli <bonelli@antifork.org>
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

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "config.h"
#include "local.h"
#include "global.h"
#include "prototype.h"
#include "macro.h"

int
difftimeofday(struct timeval now, struct timeval old, elapsed_val * diff)
{
	register int r1;
	register int r2;
	register int r3;

	r1 = (now.tv_usec - old.tv_usec) / 10000;
	r2 = (now.tv_sec - old.tv_sec) % 60;
	r3 = (now.tv_sec - old.tv_sec) / 60;

	if (r1 < 0) {
		r1 += 100;
		r2--;
		if (r2 < 0) {
			r2 += 60;
			r3--;
		}
	}
	diff->dec = r1;
	diff->sec = r2;
	diff->min = r3;

	return 0;
}


void
hash_perf()
{
	struct timeval start;
	struct timeval end;
	elapsed_val diff;
	unsigned long h;
	double r;
	double d;

	if (drv.open(media) == -1)
		FATAL("open interface error!");

	gettimeofday(&start, NULL);
	drv.reset();

	/* processing the dictionary */

	while (drv.read(buf, 79) != NULL) {
		h = (unsigned long) ext_hash(buf, strlen(buf));
	}

	gettimeofday(&end, NULL);
	difftimeofday(end, start, &diff);

	d = (double) (end.tv_sec - start.tv_sec) + (double) (end.tv_usec - start.tv_usec) / 1000000;
	r = (d ? (double) drv.w_read / d : 999999999);

	PUTS("overal_time         : %02d:%02d,%02d\n", diff.min, diff.sec, diff.dec);
	PUTS("megahash/sec        : %g\n", r / 1000000);

	drv.close();
	return;
}

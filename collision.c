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


#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "config.h"
#include "collision.h"

#include "local.h"
#include "global.h"
#include "prototype.h"
#include "macro.h"

struct hlist {
     	unsigned long hash;
	unsigned char *zero;
};            

int
hash_collision()
{
	unsigned char *p;
	unsigned long h;
	struct hlist *hlist;
	unsigned long col;
	unsigned long total_len;
	unsigned long segment_len;
	unsigned long bitmask;
	int i, j;

	if (drv.open(media) == -1)
		FATAL("open interface error!");

	col = 0;
	bitmask = (bitlen == 32 ? -1 : (1 << bitlen) - 1);
	total_len = ((bitlen > 23) ? 1 << (bitlen - 23) : 1);

	segment_len = MIN(regsize, total_len);
	if (rlimit_data != RLIM_INFINITY)
		segment_len = MIN(rlimit_data >> 21, segment_len);	/* half size of
									 * rlimit_data or
									 * regsize */

	hlist = (struct hlist *)malloc(drv.w_max * sizeof(struct hlist));

	if (hlist == NULL)
		FATAL("malloc() irrecoverable error");

	drv.reset();		/* resetting interface */

	for (j = 0; j < drv.w_max; j++) {
		if (drv.read(buf, 79) == NULL)
			FATAL("irrecoverable error");

		hlist[j].hash = hash.drv(buf);
		hlist[j].zero = NULL;
	}

	drv.close();		/* close input interface */

	do {
		p = calloc(segment_len, 1 Mbyte);
	}
	while (p == NULL && (segment_len >>= 1));

	if (p == NULL)
		FATAL("calloc() irrecoverable error");

	PUTS("global register-size: %lu Mbyte (required for %lu-bit mask)\n", total_len, bitlen);
	PUTS("segment-size        : %lu Mbyte\n", segment_len);
	PUTS("bitlen              : %lu\n", bitlen);
	PUTS("hash-bitmask        : 0x%lx\n", bitmask);

	for (i = 0; i < MAX(total_len / segment_len, 1); i++) {

		PUTS("step # %d/%d   \r", i + 1, (int) (total_len / segment_len));

		if (i > 0)
			for (j = 0; j < drv.w_max; j++) {
				if (hlist[j].zero == NULL)
					continue;
				
				*hlist[j].zero = 0;
				hlist[j].zero = NULL;
			}                        
		
		/* processing the dictionary */
		for (j = 0; j < drv.w_max; j++) {

			h = hlist[j].hash;

			if (CHECK_BOUND(i * (segment_len Mbyte), h >> 3, (i + 1) * (segment_len Mbyte) - 1)) {
				if (TST_BIT(p, i * (segment_len Mbyte), h))
					col++;

				SET_BIT(p, i * (segment_len Mbyte), h);
				
				hlist[j].zero = BASE_PTR(p, i * (segment_len Mbyte), h);
			}
		}

		/* NEXT */
	}

	free(hlist);
	free(p);
	PUTS("total collisions    : %lu/%d\n", col, drv.w_max);

	return (col);

}

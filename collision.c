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

static const char cvsid[] = "$Id$";

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

#if defined(HAVE_BSD_SYS_QUEUE_H)
#include <sys/queue.h>
#else
#include <missing/sys/queue.h>
#endif

SLIST_HEAD(hlist, hentry);

struct hentry {
	unsigned long hash;
	     SLIST_ENTRY(hentry) list;
};

int
hash_collision()
{
	unsigned char *p, **zero;
	struct hlist *hl;
	struct hentry *he, *helist;
	unsigned long sbit, total_len, segment_len, bitmask;
	int i, j, coll, steps, z;

	if (drv.open(media) == -1)
		FATAL("open interface error!");

	coll = 0;
	bitmask = (bitlen == 32 ? -1 : (1 << bitlen) - 1);
	total_len = ((bitlen > 23) ? 1 << (bitlen - 23) : 1);

	/* half size of  rlimit_data or regsize */
	segment_len = MIN(regsize, total_len);
	if (rlimit_data != RLIM_INFINITY)
		segment_len = MIN(rlimit_data >> 21, segment_len);

	drv.reset();		/* resetting interface */

	helist = (struct hentry *) malloc(drv.w_max * sizeof(struct hentry));
	zero = (unsigned char **) calloc(drv.w_max, sizeof(unsigned char *));

	do {
		p = calloc(segment_len, 1 Mbyte);
	}
	while (p == NULL && (segment_len >>= 1));

	if (p == NULL)
		FATAL("calloc() irrecoverable error");

	steps = MAX(total_len / segment_len, 1);
	for (sbit = 33, i = steps; i != 0; i >>= 1)
		sbit--;

	hl = (struct hlist *) malloc(steps * sizeof(struct hlist));

	if (hl == NULL || zero == NULL || helist == NULL)
		FATAL("malloc() irrecoverable error");

	for (j = 0; j < drv.w_max; j++) {
		if (drv.read(buf, 79) == NULL)
			FATAL("irrecoverable error");

		helist[j].hash = hash.drv(buf);

		SLIST_INSERT_HEAD(hl + (helist[j].hash >> sbit),
				  helist + j, list);
	}

	drv.close();		/* close input interface */

	PUTS("global register-size: %lu Mbyte (required for %lu-bit mask)\n",
	     total_len, bitlen);
	PUTS("segment-size        : %lu Mbyte\n", segment_len);
	PUTS("bitlen              : %lu\n", bitlen);
	PUTS("hash-bitmask        : 0x%lx\n", bitmask);

	for (i = 0; i < steps; i++) {

		PUTS("step # %d/%d   \r", i + 1, steps);

		z = 0;

		/* processing the dictionary */
		SLIST_FOREACH(he, hl + i, list) {
			if (TST_BIT(p, i * (segment_len Mbyte), he->hash))
				coll++;

			SET_BIT(p, i * (segment_len Mbyte), he->hash);

			zero[z++] = BASE_PTR(p, i * (segment_len Mbyte),
					     he->hash);
		}

		/* reset memory */
		for (j = 0; j < z; j++)
			*zero[j] = 0;

		/* NEXT */
	}

	free(helist);
	free(hl);
	free(zero);
	free(p);
	PUTS("total collisions    : %d/%d\n", coll, drv.w_max);

	return (coll);

}

/*
 * $Id$
 * framework to collect and present data to xgobi application.
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
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "config.h"

#include "local.h"
#include "global.h"
#include "prototype.h"
#include "macro.h"
#include "thash.h"

#define X_MASK	0x00ff		/* 0000000011111111 */
#define Y_MASK	0xff00		/* 1111111100000000 */


void
hash_2d()
{
	unsigned long h, x, y;
	static unsigned long sh;

	if (drv.open(media) == -1)
		FATAL("open interface error!");

	drv.reset();

	while (drv.read(buf, 79) != NULL) {

		h = ext_hash(buf, strlen(buf));

		if (opt_options & OPT_DIFF) {
			x = ((h - sh) & X_MASK);
			y = ((h - sh) & Y_MASK) >> 8;
			sh = h;
		} else {
			x = (h & X_MASK);
			y = (h & Y_MASK) >> 8;

		}

		printf("%lu %lu\n", x, y);

	}

	drv.close();

}

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
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "thash.h"
#include "macro.h"

unsigned long
__hash_pure(char *h)
{
	return (HASH(h));
}


unsigned long
__hash_mask(char *h)
{
	return (HASH(h) & hash.mask);
}


unsigned long
__hash_xor_folding(char *h)
{
	unsigned long hh;
	unsigned long ret;

	if (hash.len == 32)
		return (HASH(h));

	hh = HASH(h);
	ret = 0;

	while (hh) {
		ret ^= (hh & hash.mask);
		hh >>= hash.len;
	}

	return (ret);

}

unsigned long
__hash_dumb_mod(char *h)
{
	return (HASH(h) % hash.len);
}


#define FNV_32_PRIME            16777619
#define FNV_32_INIT             (0x811c9dc5)
#define MAX_32BIT               ((unsigned int)0xffffffff)	/* largest 32 bit
								 * unsigned value */
#define RETRY_LEVEL             ((MAX_32BIT / hash.len) * hash.len)

unsigned long
__hash_retry(char *h)
{
	unsigned long hh;
	hh = HASH(h);

	while (hh > RETRY_LEVEL) {
		hh = (hh * FNV_32_PRIME) + FNV_32_INIT;
	}

	hh %= hash.len;
	return (hh);
}


int
setup_hdriver()
{

	switch (opt_filters) {
	case 0:
		hash.drv = __hash_pure;
		break;
	case OPT_MASK:
		PUTS("drv:hash method     : mask\n");
		hash.drv = __hash_mask;
		hash.len = bitlen;
		hash.mask = bitlen == 32 ? -1 : (1 << bitlen) - 1;
		break;
	case OPT_XOR:
		PUTS("drv:hash method     : xor-folding\n");
		hash.drv = __hash_xor_folding;
		hash.len = bitlen;
		hash.mask = bitlen == 32 ? -1 : (1 << bitlen) - 1;
		break;
	case OPT_MOD:
		PUTS("drv:hash method     : lazy mod mapping\n");
		hash.drv = __hash_dumb_mod;
		hash.len = tablen;
		break;
	case OPT_RETRY:
		PUTS("drv:hash method     : fnv-retry\n");
		hash.drv = __hash_retry;
		hash.len = tablen;
		break;
	default:
		FATAL("unknown hash method");
	}

	return 0;
}

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
#include <fcntl.h>

#include "global.h"
#include "thash.h"
#include "macro.h"

int openw(char *);
int openr(char *);
int openi(char *);
int openg(char *);

char *readw(char *, int);
char *readr(char *, int);
char *readr64(char *, int);
char *readi(char *, int);
char *readg(char *, int);

int resetw();
int resetr();
int reseti();
int resetg();

int closew();
int closer();
int closei();
int closeg();

int
setup_driver()
{

	switch (opt_input) {
	case OPT_WORDLIST:
		PUTS("drv:input method    : wordlist\n");
		drv.open = openw;
		drv.read = readw;
		drv.reset = resetw;
		drv.close = closew;
		break;
	case OPT_RAND:
		PUTS("drv:input method    : ISAAC random geneator\n");
		drv.open = openr;
		drv.read = (opt_options & OPT_RADIX64 ? readr64 : readr);
		drv.reset = resetr;
		drv.close = closer;
		break;
	case OPT_GRAY:
		PUTS("drv:input method    : gray sequence\n");
		drv.open = NULL;
		drv.read = NULL;
		drv.reset = NULL;
		drv.close = NULL;
		break;
	default:
		FATAL("unknown device");
	}

	return 0;
}

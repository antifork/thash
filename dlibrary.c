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

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#include "config.h"
#include "thash.h"

#include "local.h"
#include "global.h"

#include "macro.h"
#include "prototype.h"
#include "neo_options.h"


void
hash_register(char *so)
{

	char soname[80] = "\0";
	void *handle;

	if (so == NULL)
		FATAL("err: so (NULL)");

	if (so[0] != '.' && so[0] != '/')
		strlcat(soname, "./", 80);

	strlcat(soname, so, 80);
	strlcat(soname, ".so", 80);

	handle = dlopen(soname, RTLD_NOW);

	if (!handle) {
		fputs(dlerror(), stderr);
		fputs("\n", stderr);
		exit(1);
	}
#if defined(__OpenBSD__)
	ext_hash = dlsym(handle, "_hash");
#else
	ext_hash = dlsym(handle, "hash");
#endif

	return;

}

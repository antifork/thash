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
#include <sys/mman.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "global.h"

int
openw(const char *pathname)
{

	struct stat f_stat;
	int size;
	char *p;

	if (stat(pathname, &f_stat) == -1) {
		fprintf(stderr, "open_fi::stat(%s) %s\n", pathname, strerror(errno));
		return -1;
	}
	size = (int) f_stat.st_size;

	if ((drv.fd = open(pathname, O_RDONLY)) == -1) {
		fprintf(stderr, "open_fi::open(%s) %s\n", pathname, strerror(errno));
		return -1;
	}
	drv.image = mmap(NULL, size, PROT_READ, MAP_PRIVATE, drv.fd, 0);
	drv.size = size;

	if (drv.image == MAP_FAILED) {
		perror("open_fi::mmap");
		return -1;
	}
	if (drv.w_max < 1) {
		p = (char *) drv.image;
		drv.w_max = 0;

		while (*p != '\0')
			if (*p++ == '\n')
				drv.w_max++;
	}
	return 0;
}


int
closew()
{
	drv.image = NULL;
	drv.fd = 0;

	return (munmap(drv.image, drv.size));

}


int
resetw()
{

	drv.stream = drv.image;
	drv.w_read = 0;
	return 0;
}


char *
readw(char *dst, int size)
{

	char *p = drv.stream;
	char *s = dst;

	int i = 0;

	if (p == NULL)
		return NULL;

	if (*p == 0)
		return NULL;	/* EOF */

	if (drv.w_read == drv.w_max)
		return NULL;

	while (*p != '\0' && *p != '\n' && i < size) {
		*(s++) = *(p++);
		i++;

	}

	if (*p == '\0' || *p == '\n') {
		*s = 0;
		i++;
	}
	drv.stream += i;
	drv.w_read++;

	return dst;
}

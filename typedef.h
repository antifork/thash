/*
 * $Id$
 *
 * Copyright (c) 2002 Nicola Bonelli <bonelli@antifork.org>
 * 
 * All rights reserved.
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

#ifndef TYPEDEF_H
#define TYPEDEF_H

typedef struct {
    int min;
    int sec;
    int dec;
} elapsed_val;

typedef struct {
    void *image;
    void *stream;
    int size;
    int line;
} fi_stream;


typedef struct {
    int (*open) (char *);
    int (*reset) ();
    char *(*read) (char *, int);
    int (*close) ();


    /* fileimage */

    void *image;
    void *stream;
    int size;
    int fd;

    /* random gen */

    int seed;
    int w_len;			/* wordlen */
    int w_max;			/* limit */
    int w_read;			/* counter */

} tdriver;


typedef struct {
    unsigned long (*drv) (char *);
    unsigned long len;
    unsigned long mask;
} hdriver;

#endif /* TYPEDEF_H */

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

#ifndef THASH_H
#define THASH_H

#ifdef __OpenBSD__
// The below define is a lie since we are really doing RTLD_LAZY since the
// system doesn't support RTLD_NOW.
#define RTLD_NOW DL_LAZY
#endif


/* hash source */
#define OPT_SOURCE      (1<<0)
#define OPT_SHARED      (1<<1)

/* hash filters */
#define OPT_MASK	(1<<0)
#define OPT_XOR		(1<<1)
#define OPT_MOD		(1<<2)
#define OPT_RETRY	(1<<3)

/* input */
#define OPT_WORDLIST    (1<<0)
#define OPT_RAND        (1<<1)
#define OPT_GRAY        (1<<2)

/* general */
#define OPT_SEGMENT     (1<<0)
#define OPT_DIFF        (1<<1)
#define OPT_SEED        (1<<2)
#define OPT_RADIX64     (1<<3)

/* test */
#define OPT_SEARCH      (1<<0)
#define OPT_TABLE       (1<<1)
#define OPT_COLLISION   (1<<2)
#define OPT_BLACKLIST   (1<<3)
#define OPT_2D          (1<<4)
#define OPT_3D          (1<<5)
#define OPT_PERF        (1<<6)


#endif /* THASH_H */

/*
 * general macro  
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

#ifndef MACRO_H
#define MACRO_H

#define bit         >>3
#define Kbit        <<7
#define Mbit        <<17
#define Gbit        <<27

#define Kbyte       <<10
#define Mbyte       <<20
#define Gbyte       <<30

#define FATAL(f,arg...) {                                               \
fprintf(stderr,"%s:%d: ",__FILE__,__LINE__);          			\
fprintf(stderr,f,## arg);                                               \
fprintf(stderr,"\n");                                                   \
exit(1);                                                                \
}

#define PUTS(f,arg...)  ({                                              \
fprintf(stderr,f,## arg);                                               \
fflush (stderr);                                                        \
})

#ifdef  EADBUG
#define DEBUG(f,arg...)  ({                                             \
fprintf(stderr,"%s:%d: ",__FILE__,__LINE__);          			\
fprintf(stderr,f,## arg);                                               \
fflush (stderr);                                                        \
})
#else
#define DEBUG(f,arg...) {}
#endif

#define SET(x,y) ( x |=  y )
#define CLR(x,y) ( x &= ~y )
#define TST(x,y) ( x &   y )


#define MALLOC(p,s) {						\
(p) = ( typeof(p) ) malloc((s));				\
if ((p) == NULL )						\
	FATAL("malloc() virtual memory exhausted\n");		\
}

#define HASH(p) ( drv.w_len ? (unsigned long) ext_hash (p, drv.w_len) : (unsigned long) ext_hash (p, strlen(p)) )

#ifndef STR
#define STR(a)    # a
#endif

#ifndef CAT
#define CAT(a,b)  a ## b
#endif

#ifndef XCAT
#define XCAT(a,b) CAT(a,b)
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b) ? (a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b) ? (a):(b))
#endif

#ifndef SIGN
#define SIGN(x)  ( (x) > 0 ? 1  : ( (x) == 0 ?   0  :  -1  ) )
#endif

#ifndef ABS
#define ABS(x)   ( (x) < 0 ? -(x) : (x) )
#endif

#endif /* MACRO_H */

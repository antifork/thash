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

/* 
 *    structures:  
 *
 *    ,------,      ,------,
 *    | hash |----->| hash |--->
 *    `---+--'      `---+--'
 *        |             `---------------. 
 *        `,------,   ,------,           \------,   ,------,
 *         | word |-->| word |--> NULL   | word |-->| word |-- ...
 *         `------'   `------'           `------'   `------' 
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

#if defined(HAVE_BSD_SYS_QUEUE_H)
#include <sys/queue.h>
#else
#include <missing/sys/queue.h>
#endif

TAILQ_HEAD (main_list, entry) head;

     struct entry {
	 unsigned long hash;
	     SLIST_HEAD (, entry_word) head_word;
	     TAILQ_ENTRY (entry) entries;
     };


     struct entry_word {
	 char *word;
	     SLIST_ENTRY (entry_word) next;
     };


/* blacklist.c */

     void print_list (struct main_list *head);
     void free_word (struct entry *i);
     void free_list (struct main_list *head);
     void insert_word (struct main_list *h, unsigned long n, char *word);
     void insert_list (struct main_list *h, unsigned long n);
     int hash_blacklist ();


     char *wordup (char *src)
{
    char *dst;
    int i;
    int n;

    if (src == NULL)
	return NULL;

    n = (drv.w_len ? : strlen (src));
    dst = (char *) malloc (n + 1);
    dst[n] = 0;

    for (i = 0; i < n; i++)
	dst[i] = (src[i] > 31 && src[i] < 127 ? src[i] : '.');	/* filter for printable chars */

    return (dst);

}


void
print_list (struct main_list *head)
{
    struct entry *n1;
    struct entry_word *r1;

    TAILQ_FOREACH (n1, head, entries) {
	PUTS ("(%lu): ", n1->hash);

	SLIST_FOREACH (r1, &(n1->head_word), next) {
	    PUTS ("{%s} ", r1->word);
	}

	PUTS ("\n");
    }

    return;
}


void
free_word (struct entry *i)
{
    struct entry_word *r;

    while (SLIST_FIRST (&(i->head_word)) != NULL) {
	r = SLIST_FIRST (&(i->head_word));
	SLIST_REMOVE_HEAD (&(i->head_word), next);
	free (r->word);
	free (r);
    }

    return;
}

void
free_list (struct main_list *head)
{
    struct entry *l;


    while (TAILQ_FIRST (head) != NULL) {
	l = TAILQ_FIRST (head);
	TAILQ_REMOVE (head, TAILQ_FIRST (head), entries);
	free_word (l);
	free (l);
    }

    return;
}

void
insert_word (struct main_list *h, unsigned long n, char *word)
{
    struct entry *n1;
    struct entry_word *new;

    TAILQ_FOREACH (n1, h, entries) {
	if (n == n1->hash)
	    break;
    }

    if (n1 == NULL)
	FATAL ("internal error: hash not found?!?!");

    MALLOC (new, sizeof (struct entry_word));
    new->word = wordup (word);
    SLIST_INSERT_HEAD (&(n1->head_word), new, next);

    return;

}

void
insert_list (struct main_list *h, unsigned long n)
{
    struct entry *n1;
    struct entry *new;

    TAILQ_FOREACH (n1, h, entries) {
	if (n <= n1->hash)
	    break;
    }

    if (n1 != NULL && n == n1->hash)
	return;

    MALLOC (new, sizeof (struct entry));
    new->hash = n;

    new->head_word.slh_first = NULL;

    if (n1 == NULL) {

	if (TAILQ_FIRST (h) == NULL)
	    TAILQ_INSERT_HEAD (h, new, entries);
	else
	    TAILQ_INSERT_TAIL (h, new, entries);

	return;

    }

    TAILQ_INSERT_BEFORE (n1, new, entries);
    return;

}


int
hash_blacklist ()
{
    struct entry *n1;
    unsigned char *p;
    unsigned long h;
    unsigned long col;
    unsigned long segcol;
    unsigned long total_len;
    unsigned long segment_len;
    unsigned long bitmask;
    int i;

    if (drv.open (media) == -1)
	FATAL ("open interface error!");

    col = 0;
    bitmask = (bitlen == 32 ? -1 : (1 << bitlen) - 1);
    total_len = ((bitlen > 23) ? 1 << (bitlen - 23) : 1);

    segment_len = MIN (regsize, total_len);

    if (rlimit_data != RLIM_INFINITY)
	segment_len = MIN (rlimit_data >> 21, segment_len);	/* half size of
								 * rlimit_data or
								 * regsize */
    do {
	p = malloc (segment_len Mbyte);
    }
    while (p == NULL && (segment_len >>= 1));

    if (p == NULL)
	FATAL ("malloc() irrecoverable error");


    PUTS ("global register-size: %lu Mbyte (required for %lu-bit mask)\n", total_len, bitlen);
    PUTS ("segment-size        : %lu Mbyte\n", segment_len);
    PUTS ("bitlen              : %lu\n", bitlen);
    PUTS ("hash-bitmask        : 0x%lx\n", bitmask);


    for (i = 0; i < MAX (total_len / segment_len, 1); i++) {

	memset (p, 0, segment_len Mbyte);
	drv.reset ();
	segcol = 0;

	fprintf (stderr, "step # %d/%d\n", i + 1, (int) (total_len / segment_len));

	/* processing the dictionary */

	while (drv.read (buf, 79) != NULL) {

	    h = hash.drv (buf);

	    if (CHECK_BOUND (i * (segment_len Mbyte) + 1, h >> 3, (i + 1) * (segment_len Mbyte))) {

		if TST_BIT
		    (p, i * (segment_len Mbyte) + 1, h) {

		    /* collision detected */

		    col++;
		    segcol++;

		    insert_list (&head, h);

		    }

		SET_BIT (p, i * (segment_len Mbyte) + 1, h);

	    }
	}


	if (segcol == 0)
	    continue;

	/* extracting colliding words .. */

	memset (p, 0, segment_len Mbyte);

	TAILQ_FOREACH (n1, &head, entries) {
	    SET_BIT (p, i * (segment_len Mbyte) + 1, n1->hash);
	}

	drv.reset ();

	while (drv.read (buf, 79) != NULL) {

	    h = hash.drv (buf);

	    if (CHECK_BOUND (i * (segment_len Mbyte) + 1, h >> 3, (i + 1) * (segment_len Mbyte))) {
		if TST_BIT
		    (p, i * (segment_len Mbyte) + 1, h) {
		    insert_word (&head, h, buf);
		    }

	    }

	}

	print_list (&head);
	free_list (&head);

	/* NEXT */

    }

    free (p);
    drv.close ();

    fprintf (stderr, "total collisions    : %lu/%d\n", col, drv.w_max);

    return (col);

}

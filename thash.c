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
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "config.h"
#include "thash.h"

#include "local.h"
#define  MAIN_C
#include "global.h"

#include "macro.h"
#include "prototype.h"
#include "neo_options.h"

extern char *optarg;
extern int optind,
    opterr,
    optopt;

static struct neo_options opt[] = {

    {'-', 0, 0, NULL, "hash:"},
    {'c', required_argument, "c/co", "hash.c", "hash is a c source"},
    {'o', required_argument, "o/co", "hash.so", "hash is a shared object (-fPIC)"},

    {'-', 0, 0, NULL, "filters:"},
    {'a', required_argument, "a/axmt", "bit", "and-mask (lowbits filter)"},
    {'x', required_argument, "x/axmt", "bit", "xor-folding method"},
    {'m', required_argument, "m/axmt", "len", "lazy mod mapping"},
    {'t', required_argument, "t/axmt", "len", "retry method"},


    {'-', 0, 0, NULL, "input:"},
    {'w', required_argument, "w/wrg", "wordlist", "use wordlist"},
    {'r', required_argument, "r/wrg", "n", "n-byte random strings. (use ISAAC generator)"},
    {'g', required_argument, "g/wrg", "n", "n-byte gray-rand strings"},


    {'-', 0, 0, NULL, "options:"},
    {'y', required_argument, NULL, "Mbyte", "set the memory segment size"},
    {'e', required_argument, NULL, "seed", "set seed for random generator"},
    {'z', required_argument, NULL, "n", "set the amount of random words. (10000 default)"},
    {'d', no_argument, NULL, NULL, "yield differential hash (-2 or -3)"},
    {'6', no_argument, NULL, NULL, "apply radix64 filter to random strings"},

    {'-', 0, 0, NULL, "test:"},
    {'b', no_argument, "b/blsf23p", NULL, "calc blacklist (colliding words)"},
    {'l', no_argument, "l/blsf23p", NULL, "calc collisions"},
    {'s', no_argument, "s/blsf23p", NULL, "calc fit-table statistics"},
    {'f', no_argument, "f/blsf23p", NULL, "find hash in wordlist"},
    {'2', no_argument, "2/blsf23p", NULL, "present 2d-data for gnuplot, xgobi"},
    {'3', no_argument, "3/blsf23p", NULL, "present 3d-data for gnuplot, xgobi"},
    {'p', no_argument, "p/blsf23p", NULL, "calc hash performance"},

    {'-', 0, 0, NULL, "general:"},
    {'v', no_argument, NULL, NULL, "print version"},
    {'h', no_argument, NULL, NULL, "print this help"},
    {'@', no_argument, NULL, NULL, "show options dependencies"},

    {'+', 0, "/co", 0, 0},
    {'+', 0, "/wrg", 0, 0},
    {'+', 0, "/blsf23p", 0, 0},
    {'+', 0, "ax/blf23p", 0, 0},
    {'+', 0, "mt/s", 0, 0},

    {0, 0, 0, 0, 0}
};


void
default_settings ()
{

    drv.w_max = 10000;

}

int
main (int argc, char **argv, char **env)
{

    int exit_status;
    int es;
    char *p;

    if (argc < 2)
	FATAL ("no arguments given");

    if (argv[1][0] == '-' && argv[1][1] == '-')
	FATAL ("%s doesn't support --long-options.\ntype %s -h instead", argv[0], argv[0]);

    default_settings ();

    environ = env;

    while ((es = neo_getopt (argc, argv, opt)) != EOF)
	switch (es) {
/* hash */
	 case 'c':
	     SET (opt_hash, OPT_SOURCE);
	     hashsource = strdup (optarg);
	     break;
	 case 'o':
	     SET (opt_hash, OPT_SHARED);
	     hashsource = strdup (optarg);
	     break;
/* filters */
	 case 'a':
	     SET (opt_filters, OPT_MASK);
	     bitlen = strtoul (optarg, (char **) NULL, 0);
	     break;
	 case 'x':
	     SET (opt_filters, OPT_XOR);
	     bitlen = strtoul (optarg, (char **) NULL, 0);
	     break;
	 case 'm':
	     SET (opt_filters, OPT_MOD);
	     tablen = strtoul (optarg, (char **) NULL, 0);
	     break;
	 case 't':
	     SET (opt_filters, OPT_RETRY);
	     tablen = strtoul (optarg, (char **) NULL, 0);
	     break;
/* input */
	 case 'w':
	     SET (opt_input, OPT_WORDLIST);
	     media = strdup (optarg);
	     break;
	 case 'r':
	     SET (opt_input, OPT_RAND);
	     drv.w_len = strtoul (optarg, (char **) NULL, 0);
	     break;
	 case 'g':
	     SET (opt_input, OPT_GRAY);
	     drv.w_len = strtoul (optarg, (char **) NULL, 0);
	     break;
/* options */
	 case 'y':
	     SET (opt_options, OPT_SEGMENT);
	     usrsize = strtol (optarg, (char **) NULL, 0);
	     if (mit_bitcount (usrsize) != 1)
		 FATAL ("segment size must be a power of 2");
	     break;
	 case 'e':
	     SET (opt_options, OPT_SEED);
	     drv.seed = strtoul (optarg, (char **) NULL, 0);
	     break;
	 case 'z':
	     drv.w_max = strtoul (optarg, (char **) NULL, 0);
	     break;
	 case 'd':
	     SET (opt_options, OPT_DIFF);
	     break;
	 case '6':
	     SET (opt_options, OPT_RADIX64);
	     break;
/* test */
	 case 'b':
	     SET (opt_test, OPT_BLACKLIST);
	     break;
	 case 'l':
	     SET (opt_test, OPT_COLLISION);
	     break;
	 case 's':
	     SET (opt_test, OPT_TABLE);
	     break;
	 case 'f':
	     SET (opt_test, OPT_SEARCH);
	     findhash = strtoul (optarg, (char **) NULL, 0);
	     break;
	 case '2':
	     SET (opt_test, OPT_2D);
	     break;
	 case '3':
	     SET (opt_test, OPT_3D);
	     break;
	 case 'p':
	     SET (opt_test, OPT_PERF);
	     break;
/* general */
	 case 'v':
	     PUTS ("thash %s (segment_size=%d Mbyte)\n", VERSION, REGSIZE);
	     exit (0);
	     break;
	 case 'h':
	     neo_usage (stderr, "usage: thash [options]", opt);
	     exit (1);
	     break;
	 case '@':
	     neo_showdepend (stderr, opt);
	     exit (0);
	 case '?':
	     exit (0);
	     break;
	}

    argc -= optind;
    argv += optind;

    /* bitlen test */
    bitlen = ((bitlen <= 32) ? bitlen : 32);

    /* getting rlimit */
    getlimit_data ();

    /* register (segment) size can't exceed the hardcoded value. REGSIZE */
    regsize = MIN (regsize, usrsize);

    if (rlimit_data == RLIM_INFINITY)
	PUTS ("RLIMIT_DATA         : (infinity)\n");
    else
	PUTS ("RLIMIT_DATA         : %llu Mbyte\n", (rlimit_data >> 20));

    /* setup drivers */

    setup_driver ();
    setup_hdriver ();

    /* removing extension from hash filename */
    p = strrchr (hashsource, '.');

    if (p != NULL) {

	if ((p[1] == 'c' && p[2] == '\0') || (p[1] == 's' && p[2] == 'o' && p[3] == '\0'))
	    *p = 0;
    }

    /* compiling hash.c if required ... */
    if (TST (opt_hash, OPT_SOURCE)) {

	PUTS ("compiling           : %s.c ... ", hashsource);

	exit_status = cc_call (hashsource);

	switch (WEXITSTATUS (exit_status)) {
	 case 0:
	     /* ok */
	     break;
	 case 1:
	     FATAL ("cc: fatal error");
	     break;
	 case 127:
	     FATAL ("cc: compiler not found");
	     break;

	}

	PUTS ("(ok)\n");
    }

    /* registering the dynamic library  */

    PUTS ("loading             : %s.so ... ", hashsource);
    hash_register (hashsource);
    PUTS ("(done)\n");

    switch (opt_test) {

     case OPT_SEARCH:
	 hash_search ();
	 break;
     case OPT_TABLE:
	 hash_fitest ();
	 break;
     case OPT_COLLISION:
	 hash_collision ();
	 break;
     case OPT_BLACKLIST:
	 hash_blacklist ();
	 break;
     case OPT_2D:
	 hash_2d ();
	 break;
     case OPT_3D:
	 hash_3d ();
	 break;
     case OPT_PERF:
	 hash_perf ();
	 break;
    }

    exit (1);

}

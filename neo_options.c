/* $Id$
*/
/*
 * neo_options
 *
 * Copyright (c) 2002 Bonelli Nicola <bonelli@antifork.org>
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

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static const char cvsid[] = "$cvsid: neo_options.c,v 1.11 2003/05/05 00:46:24 awgn Exp $";
static const char copyright[] = "Copyright (c) 2002 Bonelli Nicola <bonelli@antifork.org>";

extern char *__progname;

#define   NEO_LIBRARY
#include "neo_options.h"
#define RSIZE	(80 bit)


static void fatal(char *pattern,...) __attribute__((noreturn));
static void
fatal(char *pattern,...)
{
	va_list ap;
	va_start(ap, pattern);
	vfprintf(stderr, pattern, ap);
	va_end(ap);
	exit(1);
}

static int
check_neo_options(struct neo_options * n)
{
	REG reg[RSIZE];
	REG_CLR(reg, RSIZE);

	for (; n->opt != 0; n++)
		switch (n->opt) {
		case '-':
		case '+':
			continue;
		default:
			if (REG_BT(reg, (n->opt - '0')))
				fatal("[+] %s: -%c opt error. option already in use.\n", __FUNCTION__ ,n->opt);
			REG_BS(reg, (n->opt - '0'));
		}
	return 0;
}

static int
compile_optmask(char c, char *s, REG r[], REG m[])
{
        REG reg[RSIZE];
	int ret = 0, sep = 0, type=0, i;

	REG_CLR(r, RSIZE);
	REG_CLR(m, RSIZE);

	if (s == NULL)
		return 0;

	for (i = 0; s[i] != 0; i++) {
		switch (s[i]) {
		case '|':
			type = OR_MASK;
			if (c != '+')
				fatal("[+] %s: wrong separator at \"%s\" (and-mask format is \"x/xy\").\n", 
					__FUNCTION__,s);  /* error */
                        if (sep != 0)
                                fatal("[+] %s: parse error at \"%s\" (or-mask).\n", __FUNCTION__,s);  /* error */
			sep++;
			continue;
		case '/':
			type = AND_MASK;
                        if (c == '+')
                                fatal("[+] %s: wrong separator at \"%s\" (or-mask format is \"x|yz\").\n", 
                                        __FUNCTION__,s);  /* error */
			if (sep != 0)
				fatal("[+] %s: -%c parser error at \"%s\" (and-mask).\n", __FUNCTION__,c, s);	/* error */
			sep++;
			continue;
		default:
			REG_BS((sep == 0 ? r : m), (s[i] - '0'));
			ret |= (sep == 0 ? 2 : 1);
		}
	}

	/* mask integrity check */

	if ( type == AND_MASK ) {
		REG_CPY(m,reg,RSIZE);
		REG_AND(r,reg,RSIZE);
		if (!REG_CMP(r,reg,RSIZE)) 
			fatal("[+] %s: -%c \"%s\" bogus and-mask.\n", __FUNCTION__,c, s); /* error */
	}

        if ( type == OR_MASK && !REG_ISNULL(r,RSIZE)) {
                REG_CPY(m,reg,RSIZE);
                REG_AND(r,reg,RSIZE);
                if (REG_CMP(r,reg,RSIZE))
                        fatal("[+] %s: \"%s\" bogus or-mask.\n", __FUNCTION__,s); /* error */
        }

	return (ret);
}


static char *
compile_optstring(struct neo_options * n)
{
	struct neo_options *m;
	char *op;
	int l = 0;

	for (m = n; m->opt != 0; m++)
		l += (m->has_arg > 1 ? 2 : 1);
	op = (char *) malloc(l + 1);
	op[l] = 0;
	for (m = n, l = 0; m->opt != 0; m++)
		switch (m->has_arg) {
		case 0:
			continue;
		case 1:
			op[l++] = m->opt;
			continue;
		default:
			op[l++] = m->opt;
			op[l++] = ':';
		}

	return op;
}


static char spaces[32] = { [0 ... 31] = ' '};

/***
 *** public functions
 ***/

int
neo_usage(FILE * f, char *h, struct neo_options * n)
{
	int br = 1;

	if (n == NULL)
		fatal("[+] %s: struct neo_options is a NULL pointer?\n",__FUNCTION__);

	/* print header */

	if (h != NULL)
		fprintf(f, "%s\n", h);
	else
		fprintf(f, "usage: %s [OPTIONS]\n",__progname);

	for (; n->opt != 0; n++)
		switch (n->opt) {
		case '-':
			fprintf(f, " %s\n", n->usage);
			continue;
		case '+':
			continue;
		default:
			if (n->usage != NULL) {
				if (n->arg == NULL) {
					fprintf(f, "   -%c                           %s\n", n->opt, n->usage);
					break;
				}
				spaces[(sizeof(spaces) - br) & 0x1f] = ' ';
				fprintf(f, "   -%c %s%n", n->opt, n->arg, &br);
				spaces[(sizeof(spaces) - br) & 0x1f] = '\0';
				fprintf(f, "%s%s\n", spaces, n->usage);
			}
		}
	return 0;
}

int
neo_showdepend(FILE * f, struct neo_options * n)
{

	if (n == NULL)
		fatal("[+] %s: struct neo_options is a NULL pointer?\n",__FUNCTION__);

	fprintf(f, "%s options dependencies:\n",__progname);
	for (; n->opt != 0; n++)
		switch (n->opt) {
		case '-':
		case '!':
			continue;
		case '+':
			if (n->depend != NULL)
				fprintf(f, "      (or)                      \"%s\"\n", n->depend);
			continue;
		default:
			if (n->depend != NULL)
				fprintf(f, "   -%c (and)                     \"%s\"\n", n->opt, n->depend);
		}
	return 0;
}

int
neo_getopt(int argc, char *const argv[], struct neo_options * n, int mode)
{
	REG opts[RSIZE], mask[RSIZE], temp[RSIZE];
	static REG master[RSIZE];
	static char *optstring;
	int c, d;

	if (optstring == NULL) {
		/* init */
		check_neo_options(n);
		optstring = compile_optstring(n);
	}

	switch(mode) {
	case OPT_NOW: /* check all options concistency */
                while (( c = getopt(argc, argv, optstring))!= EOF )     {

                        if ( c == 'h' ) { // stop the check now 
                                optind=1;
                                return (c);
                        }
                        REG_BS(master, (c - '0'));
                }
                optind = 1;
		break;
	case OPT_DELAYED:
                c = getopt(argc, argv, optstring);
                if (c != EOF) {
                        REG_BS(master, (c - '0'));
                        return (c);
                }
		break;
	default:
		fatal("[+] %s: unknown mode.\n", __FUNCTION__); 	
	}

	for (; n->opt != 0; n++) {
		switch (n->opt) {
			/* comment line */
		case '-':
			continue;
			/* obligation line */
		case '+':
			REG_CPY(master, temp, RSIZE);
			d = compile_optmask(n->opt, n->depend, opts, mask);

			switch (d) {
			 /* NULL/NULL */ case 0:
				continue;
			 /* NULL/OK */ case 1:
				break;
			 /* OK/NULL */ case 2:
				/* error ignoring it... */
				continue;
			 /* OK/OK */ case 3:
				/*
				 * control whether to check the
				 * obligation or not
				 */

				REG_AND(temp, opts, RSIZE);

				if (REG_ISNULL(opts, RSIZE))
					continue;
			}

			/* obligation(or) mask check */
			REG_AND(mask, temp, RSIZE);
			if (REG_ISNULL(temp, RSIZE))
				fatal("[!] %s: or-mask \"%s\" error. (missing options?)\n", 
					__FUNCTION__,n->depend);
			continue;

			/* (and) mask check */
		default:
			if (REG_BT(master, (n->opt - '0')) == 0)
				continue;	/* the current options wasn't
						 * set, its optmask is
						 * ignored */

			/* be sure that the current options is set */
			REG_CPY(master, temp, RSIZE);
			REG_BS(temp, (n->opt - '0'));
			compile_optmask(n->opt, n->depend, opts, mask);
			REG_AND(mask, temp, RSIZE);

			if (!REG_CMP(temp, opts, RSIZE)) /* error */
				fatal("[!] %s: -%c and-mask \"%s\" error. (mismatch dependency?)\n", 
					__FUNCTION__ ,n->opt, n->depend);
			continue;
		}

	}
	return (c);
}


#ifdef TEST

struct neo_options opt[] = {
	{'-', 0, 0, NULL, "first section"},
	{'a', no_argument, "a/ab", NULL, "test"},
//	{'a', no_argument, "a/cb", NULL, "test"}, <- bugus and-mask (never true) (recognized by mask compiler) 
//	{'k', no_argument, "a|cb", NULL, "test"}, <- wrong separator: and-mask format is "a/ab"
	{'b', no_argument, "bc/bc", NULL, "test.."},
	{'-', 0, 0, NULL, "second section"},
	{'c', required_argument, "cd/cd", "int", "test..."},
	{'d', required_argument, "d/d", "u_int", "test...."},
	{'e', no_argument, "", NULL, "---"},
	{'z', no_argument, "", NULL, "test....."},
	{'h', no_argument, "", NULL, "print help"},
	{'!', no_argument, NULL, NULL, "show dependencies"},
	{'+', 0, "az|de", 0},
//	{'+', 0, "a|ade", 0}, <- bogus or-mask (always true) (recognized by mask compiler)
//	{'+', 0, "a/bc", 0 }, <- wrong separator: or-mask format is "x|yz" 
	{'+', 0, "|dz", 0},
	{0, 0, 0, 0}
};

int
main(int argc, char **argv)
{
	int i;

//	int
//	neo_getopt(int argc, char **argv, struct neo_options array[], int MODE);
//	
//	argc, argv: number and command line arguments passed to main.
//	struct neo_options array[]: array of config structure.
//
//	MODE: can be either OPT_NOW or OPT_DELAYED.  
//
//	     *OPT_NOW    : check all options given in a single step.
//	      OPT_DELAYED: check a single option per call (used for multiple iterance).
//
//     *BUGS: If an or-mask "|xxx" was defined (pure obligation or-mask), the 
//	      neo_getopt(...,OPT_NOW) failed when no options are passed to the command line. 
//	      To avoid this beaviour and permit a subsequent usage+exit call, when -h is given, 
//	      we implicit assume that masks check are not relevant, and neo_getopt() is 
//	      forced to return.  
//	       

	printf("OPT_NOW ...\n");
	neo_getopt(argc, argv, opt, OPT_NOW);

	printf("OPT_DELAYED ...\n");
	while ((i = neo_getopt(argc, argv, opt, OPT_DELAYED)) != EOF)
		switch (i) {
		case '?':
			exit(-1);
			break;
		case 'h':
			// int
			// neo_usage(FILE *, char *custom_message, struct neo_options array[]); 
			//
			// print the usage() program in a classic fashion.
			// if custom_message is NULL, a predefined header is printed.
			//
			// ie:
			// neo_usage(stderr, "usage: prog blhablhabblha", opt);

			neo_usage(stderr, NULL, opt);
			exit(1);
			break;
		case '!':
			// int
			// neo_showdepend(FILE *, struct neo_options array[]);
			//
			// show the dependencies, printing (and) and (or) mask in a 
			// classic fashion.
			//
	
			neo_showdepend(stderr, opt);
			exit(1);
			break;
		default:
			printf("opt: %-c given\n", i);
			break;
		}

	printf("done :-)\n");
	return(0);
}
#endif

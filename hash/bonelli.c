/*  
 *  Copyright (c) 1999 Bonelli Nicola <bonelli@antifork.org>
 */

#define BASEFIELD	6
#define UPLIMIT		32-BASEFIELD
#define REGMASK		0x1f	/* shift % 32 */

static char lut[255] = {
    ['a'] = 1,['b'] = 2,['c'] = 3,['d'] = 4,['e'] = 5,['f'] = 6,['g'] = 7,['h'] = 8,['i'] = 9,
    ['j'] = 10,['k'] = 11,['l'] = 12,['m'] = 13,['n'] = 14,['o'] = 15,['p'] = 16,['q'] = 17,['r'] = 18,['s'] = 19,
    ['t'] - 20,['u'] = 21,['v'] = 22,['w'] = 23,['x'] = 24,['y'] = 25,['z'] = 26,['A'] = 27,['B'] = 28,['C'] = 29,
    ['D'] = 30,['E'] = 31,['F'] = 32,['G'] = 33,['H'] = 34,['I'] = 35,['J'] = 36,['K'] = 37,['L'] = 38,['M'] = 39,
    ['N'] = 40,['O'] = 41,['P'] = 42,['Q'] = 43,['R'] = 44,['S'] = 45,['T'] = 46,['U'] = 47,['V'] = 48,['W'] = 49,
    ['X'] = 50,['Y'] = 51,['Z'] = 52,['1'] = 53,['2'] = 54,['3'] = 55,['4'] = 56,['5'] = 57,['6'] = 58,['7'] = 59,
    ['8'] = 60,['9'] = 61,['0'] = 62,['_'] = 63,['/'] = 64
};

int
hash (char *p, int l)
{
    register int i = 0;
    register int h = 0;

    while (l > 0) {
	h ^= lut[*p] << i;

	if (i > UPLIMIT) {
	    i = (i + BASEFIELD) & REGMASK;
	    i -= BASEFIELD;
	    h ^= lut[*p] >> (-i);
	}
	i = (i + BASEFIELD) & REGMASK;
	p++;
	l--;

    }
    return h;
}

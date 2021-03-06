/*
 * MIT Bitcount
 *
 * Consider a 3 bit number as being 4a+2b+c if we shift it right 1 bit, we have
 * 2a+b subtracting this from the original gives 2a+b+c if we shift the
 * original 2 bits right we get a and so with another subtraction we have
 * a+b+c which is the number of bits in the original number.
 *
 * Suitable masking  allows the sums of  the octal digits  in a 32 bit  number
 * to appear in  each octal digit.  This  isn't much help  unless we can get
 * all of them summed together.   This can be done by modulo  arithmetic (sum
 * the digits in a number by  molulo the base of the number minus  one) the
 * old "casting out nines" trick  they taught  in school before  calculators
 * were  invented.  Now, using mod 7 wont help us, because our number will
 * very likely have more than 7 bits set.   So add  the octal digits
 * together to  get base64 digits,  and use modulo 63.   (Those of you  with
 * 64  bit machines need  to add 3  octal digits together to get base512
 * digits, and use mod 511.)
 *
 * This is HACKMEM 169, as used in X11 sources. Source: MIT AI Lab memo, late
 * 1970's.
 */

static const char cvsid[] = "$Id$";

int
mit_bitcount(unsigned int n)
{
	/* works for 32-bit numbers only */
	register unsigned int tmp;

	tmp = n - ((n >> 1) & 033333333333) - ((n >> 2) & 011111111111);
	return ((tmp + (tmp >> 3)) & 030707070707) % 63;
}

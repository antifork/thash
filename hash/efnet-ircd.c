/*
 * ircd-hybrid: an advanced Internet Relay Chat Daemon(ircd). hash.c:
 * Maintains hashtables.
 */

int
hash(char *p, int s)
{
	unsigned int h = 0;
	int i = 0;

	for (; i < s; i++) {
		h = (h << 4) - (h + (unsigned char) *p++);
	}

	return h;
}

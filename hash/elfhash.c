/* ElfHash */

int
hash(char *p, int s)
{
	unsigned long h = 0;
	unsigned long g = 0;
	int i = 0;

	for (; i < s; i++) {
		h = (h << 4) + p[i];
		if ((g = (h & 0xf0000000)))
			h ^= g >> 24;
		h &= ~g;

	}

	return h;
}

int
hash(char *key, int len)
{
	int hash, i;

	for (hash = 0, i = 0; i < len; ++i) {
		hash ^= (int) key[i] << ((i & 0x3) * 8);
	}
	return (hash);
}

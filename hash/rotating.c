int
hash(char *key, int len)
{
	int hash, i;

	for (hash = len, i = 0; i < len; ++i)
		hash = (hash << 4) ^ (hash >> 28) ^ key[i];
	return (hash);
}

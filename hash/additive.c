int
hash(char *key, int len)
{
	int hash, i;

	for (hash = len, i = 0; i < len; ++i)
		hash += key[i];
	return hash;
}

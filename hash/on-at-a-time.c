int
hash (char *key, int len)
{
    int hash,
        i;

    for (hash = 0, i = 0; i < len; ++i) {
	hash += key[i];
	hash += (hash << 10);
	hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return (hash);
}

#define ethernet_polynomial 0x04c11db7U

int
hash (char *data, int length)
{
    int crc = -1;

    while (--length >= 0) {
	unsigned char current_octet = *data++;
	int bit;

	for (bit = 0; bit < 8; bit++, current_octet >>= 1)
	    crc = (crc << 1) ^ ((crc < 0) ^ (current_octet & 1) ? ethernet_polynomial : 0);
    }
    return crc;
}

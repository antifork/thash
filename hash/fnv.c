/* Fowler / Noll / Vo (FNV) Hash..     
   Noll, Landon Curt  (LCN2)  lcn2-mail@ASTHE.COM
   chongo <was here> /\oo/\ 

   #define HASH(sta,end,hash) { while (end != sta)hash=((hash*0x01000193)^(*end--));}

*/

#define FNV_prime 16777619

int
hash (char *p, int s)
{
    unsigned long h = 0;
    int i = 0;

    for (; i < s; i++)
	h = ((h * FNV_prime) ^ (p[i]));

    return h;
}

#include "neo_options.h"
#include "typedef.h"

#if defined(__STDC__) || defined(__cplusplus)
# define _P(s) s
#else
# define _P(s) ()
#endif


/* bitcount.c */
int mit_bitcount _P((unsigned int n));

/* cc.c */
int cc_call _P((const char *source));

/* collision.c */
int hash_collision _P((void));

/* dlibrary.c */
void hash_register _P((char *so));

/* driver.c */
int setup_driver _P((void));

/* fit.c */
void hash_fitest _P((void));

/* gsort.c */
__inline int mit_bitcount _P((unsigned int n));
__inline int metric _P((char *a, char *b));
void gsort _P((char **table, int len));

/* hash_2d.c */
void hash_2d _P((void));

/* hash_3d.c */
void hash_3d _P((void));

/* input_rand.c */
void b3b4_radix _P((char *a, char *b));
int openr _P((char *n));
int resetr _P((void));
char *readr _P((char *b, int null));
char *readr64 _P((char *b, int null));
int closer _P((void));

/* input_wordlist.c */
int openw _P((const char *pathname));
int closew _P((void));
int resetw _P((void));
char *readw _P((char *dst, int size));

/* limit.c */
void getlimit_data _P((void));

/* neo_options.c */
int neo_usage _P((FILE *f, char *h, struct neo_options *n));
int neo_showdepend _P((FILE *f, struct neo_options *n));
int neo_getopt _P((int argc, char *const argv[], struct neo_options *n));

/* perf.c */
int difftimeofday _P((struct timeval now, struct timeval old, elapsed_val *diff));
void hash_perf _P((void));


/* search.c */
void hash_search _P((void));

/* thash.c */
void default_settings _P((void));
int main _P((int argc, char **argv, char **environ));

/* missing/strlcat.c */
size_t strlcat _P((char *dst, const char *src, size_t siz));

/* missing/strlcpy.c */
size_t strlcpy _P((char *dst, const char *src, size_t siz));

#undef _P

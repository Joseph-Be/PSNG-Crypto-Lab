/*
 * genkey.c  --  Insecure 128-bit key generator (STUDENT-FACING)
 *
 * This is the program the careless developer used to create the
 * encryption key for the confidential report.  It seeds the standard
 * C pseudo-random number generator with the CURRENT TIME.
 *
 * Study it carefully: the whole scenario hinges on why this is a
 * terrible idea for cryptographic keys.
 *
 * Build:   gcc -o genkey genkey.c
 * Run:     ./genkey
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define KEYSIZE 16

void main()
{
    int i;
    char key[KEYSIZE];

    printf("%lld\n", (long long) time(NULL));
    srand (time(NULL));          /* <-- Line A : the seed is the clock */

    for (i = 0; i < KEYSIZE; i++) {
        key[i] = rand() % 256;
        printf("%.2x", (unsigned char) key[i]);
    }
    printf("\n");
}

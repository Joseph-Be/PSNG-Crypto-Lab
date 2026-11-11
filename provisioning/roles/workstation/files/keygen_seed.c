/*
 * keygen_seed.c  --  (PROVISIONING ONLY, not given to students)
 *
 * Same insecure key-derivation logic as genkey.c, but the seed is taken
 * from argv[1] instead of time(NULL).  Provisioning uses this to compute
 * the exact key for the per-sandbox seed, so it can encrypt the secret
 * file with the SAME key the student will later recover by brute force.
 *
 * Because this is compiled and run ON the SEED workstation host, it uses
 * the very same glibc rand() sequence the student's solver will use ->
 * the recovered key is guaranteed to match.
 *
 * Build: gcc -o keygen_seed keygen_seed.c
 * Run:   ./keygen_seed <seed>      # prints 32 hex chars (16 bytes)
 */
#include <stdio.h>
#include <stdlib.h>

#define KEYSIZE 16

int main(int argc, char **argv)
{
    int i;
    unsigned char key[KEYSIZE];

    if (argc < 2) {
        fprintf(stderr, "usage: %s <seed>\n", argv[0]);
        return 1;
    }

    srand((unsigned) strtoul(argv[1], NULL, 10));

    for (i = 0; i < KEYSIZE; i++) {
        key[i] = rand() % 256;
        printf("%.2x", key[i]);
    }
    printf("\n");
    return 0;
}

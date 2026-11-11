/*
 * solve.c  --  INSTRUCTOR REFERENCE brute-force solver for Level 3.
 *
 * Known-plaintext attack against a file encrypted with AES-128-CBC whose
 * key was produced by genkey.c (srand(time(NULL))).  The file was created
 * at 2018-04-17 23:08:49 UTC; we try every 1-second seed in the 2-hour
 * window before that timestamp.
 *
 * For each candidate seed we regenerate the 16-byte key exactly as
 * genkey.c would, encrypt the known first plaintext block under AES-128-CBC
 * with the known IV, and compare the result to the first ciphertext block.
 * A match reveals the key.
 *
 * Build: gcc -o solve solve.c -lcrypto
 * Run:   ./solve
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/evp.h>

#define KEYSIZE 16

/* known first 16 bytes of the plaintext PDF header: "%PDF-1.5\n%...."  */
static const unsigned char P0[16] = {
    0x25,0x50,0x44,0x46,0x2d,0x31,0x2e,0x35,
    0x0a,0x25,0xd0,0xd4,0xc5,0xd8,0x0a,0x34
};
/* IV stored in the encrypted file (never secret) */
static const unsigned char IV[16] = {
    0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,
    0x01,0x00,0xa2,0xb2,0xc2,0xd2,0xe2,0xf2
};

static int hex2bin(const char *hex, unsigned char *out, int outlen)
{
    for (int i = 0; i < outlen; i++)
        if (sscanf(hex + 2*i, "%2hhx", &out[i]) != 1) return -1;
    return 0;
}

/* AES-128-CBC encrypt exactly one 16-byte block, no padding */
static void enc_block(const unsigned char *key, const unsigned char *in,
                      unsigned char *out)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len;
    EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, IV);
    EVP_CIPHER_CTX_set_padding(ctx, 0);
    EVP_EncryptUpdate(ctx, out, &len, in, 16);
    EVP_CIPHER_CTX_free(ctx);
}

int main(int argc, char **argv)
{
    /* first ciphertext block: pass as hex arg, or edit C0 below */
    unsigned char C0[16];
    const char *c0_hex = (argc > 1) ? argv[1] : NULL;
    if (!c0_hex) {
        fprintf(stderr, "usage: %s <first-16-ciphertext-bytes-in-hex>\n", argv[0]);
        fprintf(stderr, "  (take them from:  xxd -p -l 16 secret_report.enc)\n");
        return 1;
    }
    if (hex2bin(c0_hex, C0, 16) != 0) { fprintf(stderr, "bad hex\n"); return 1; }

    const unsigned long END   = 1524006529UL;  /* 2018-04-17 23:08:49 UTC */
    const unsigned long START = END - 7200UL;   /* 2 hours earlier        */

    unsigned char key[KEYSIZE], guess[16];
    for (unsigned long seed = START; seed <= END; seed++) {
        srand((unsigned) seed);
        for (int i = 0; i < KEYSIZE; i++) key[i] = rand() % 256;
        enc_block(key, P0, guess);
        if (memcmp(guess, C0, 16) == 0) {
            printf("[+] seed  = %lu\n", seed);
            printf("[+] key   = ");
            for (int i = 0; i < KEYSIZE; i++) printf("%02x", key[i]);
            printf("\n");
            printf("Now decrypt the whole file, e.g.:\n");
            printf("  KEY=");
            for (int i = 0; i < KEYSIZE; i++) printf("%02x", key[i]);
            printf("\n  openssl enc -d -aes-128-cbc -nopad -nosalt \\\n");
            printf("    -K $KEY -iv 09080706050403020100a2b2c2d2e2f2 \\\n");
            printf("    -in secret_report.enc | strings | grep -i flag\n");
            return 0;
        }
    }
    printf("[-] key not found in window\n");
    return 1;
}

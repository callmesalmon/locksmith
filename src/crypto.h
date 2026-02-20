#ifndef CRYPTO_H
#define CRYPTO_H

#include <sodium.h>

#define MAX_HASH_LEN 8192
#define MAX_STRING_LEN 128

#define SPECIAL_CHARS "!@#$%^&*()-_=+,.?/:;{}[]~"
#define NUMBER_CHARS "0123456789"
#define UPPER_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define LOWER_CHARS "abcdefghijklmnopqrstuvwxyz"
#define ALL_CHARS UPPER_CHARS LOWER_CHARS NUMBER_CHARS SPECIAL_CHARS

int encrypt(const char *fname, const char *password, const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]);
unsigned char *decrypt(const char *source_file, const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]);
int create_key(const char *key_file);
int hash_password(char *password, unsigned char *hash);

#endif

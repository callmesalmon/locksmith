#ifndef CRYPTO_H
#define CRYPTO_H

#define MAX_STRING_LEN 100
#define MAX_HASH_LEN 5000

int encrypt_decrypt(char *fname);
unsigned char *hash_password(char *password, unsigned char *hash);

#endif

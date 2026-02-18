#ifndef CRYPTO_H
#define CRYPTO_H

#define MAX_STRING_LEN 128
#define MAX_HASH_LEN 8192 // Please don't overflow please please please

int encrypt_decrypt(char *fname);
int hash_password(char *password, unsigned char *hash);

#endif

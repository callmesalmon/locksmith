#include "crypto.h"
#include <stdio.h>
#include <string.h>

#include "commons.h"

char *xor_cipher(char string[]) {
    // This can be any value
    char xorKey = 'E';
    
    int len = strlen(string);
    for (int i = 0; i < len; i++) {
        string[i] = string[i] ^ xorKey;
    }

    return 0;
}

int encrypt_decrypt(char *fname) {
    FILE *fptr;
    char buff[MAX_STRING_LEN];
    
    fptr = fopen(fname, "r");
    if (fptr == NULL) {
        die("Couldn't encrypt password. Failed to open file.");
    }
    fgets(buff, MAX_STRING_LEN, fptr);
    xor_cipher(buff);

    fptr = fopen(fname, "w");
    if (fptr == NULL) {
        die("Couldn't encrypt password. Failed to open file.");
    }
    fprintf(fptr, buff);
    
    fclose(fptr);

    return 0;
}

int hash_password(char *password, unsigned char *hash) {
    unsigned int hash_val = 0;
    int i = 0;
    while (password[i] != '\0') {
        hash_val += password[i];
        hash_val = (hash_val << 3) | (hash_val >> (32 - 3));
        i++;
    }

    for (i = 0; i < sizeof(unsigned int); i++) {
        hash[i] = (hash_val >> (i * 8)) & 0xFF;
    }

    return 0;
}

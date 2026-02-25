#include <stdio.h>
#include <string.h>
#include <sodium.h>

#include "crypto.h"

/* The encrypt(), decrypt() and gen_key()
 * functions are taken from pass's source code
 * (https://github.com/briandowns/pass)
 * License:
 * 
 * Copyright 2022 <Brian J. Downs>
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

int encrypt(const char *fname, const char *password, const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]) {
    unsigned char buf_out[MAX_STRING_LEN + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];

    crypto_secretstream_xchacha20poly1305_state st;

    unsigned long long out_len;
    unsigned char tag;
    
    FILE *fptr = fopen(fname, "wb");

    crypto_secretstream_xchacha20poly1305_init_push(&st, header, key);
    
    fwrite(header, 1, sizeof header, fptr);

    size_t pass_len = strlen(password);
    do {
        tag = EOF ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
        crypto_secretstream_xchacha20poly1305_push(
            &st, buf_out, &out_len, (unsigned char *)password, pass_len, NULL, 0, tag);
        fwrite(buf_out, 1, (size_t)out_len, fptr);
    } while (!EOF);

    fclose(fptr);

    return 0;
}

unsigned char *decrypt(const char *source_file, const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]) {
    unsigned char buf_in[MAX_STRING_LEN + crypto_secretstream_xchacha20poly1305_ABYTES];
    static unsigned char buf_out[MAX_STRING_LEN];
    unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];

    crypto_secretstream_xchacha20poly1305_state st;

    unsigned long long out_len;
    int eof;
    unsigned char tag;

    FILE *fp_s = fopen(source_file, "rb");
    fread(header, 1, sizeof(header), fp_s);
    
    if (crypto_secretstream_xchacha20poly1305_init_pull(&st, header, key) != 0) {
        return NULL;
    }

    do {
        size_t rlen = fread(buf_in, 1, sizeof buf_in, fp_s);
        
        eof = feof(fp_s);
        
        if (crypto_secretstream_xchacha20poly1305_pull(&st, buf_out, &out_len, &tag, buf_in, rlen, NULL, 0) != 0) {
            return NULL;
        }

        if (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && ! eof) {
            return NULL;
        }

    } while (!eof);

    fclose(fp_s);
    
    return buf_out;
}

int create_key(const char *key_file)  {
    unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];
    crypto_secretstream_xchacha20poly1305_keygen(key);

    FILE *f = fopen(key_file, "w");
    if (!f) {
        return 1;
    }

    fwrite(key, 1, crypto_secretstream_xchacha20poly1305_KEYBYTES, f);
    fclose(f);

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

    for (i = 0; i < (int)sizeof(unsigned int); i++) {
        hash[i] = (hash_val >> (i * 8)) & 0xFF;
    }

    return 0;
}

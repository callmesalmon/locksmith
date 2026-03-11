#ifndef AUTH_H
#define AUTH_H

#include <stdio.h>
#include "crypto.h"

typedef struct {
    char            password[MAX_STRING_LEN];
    unsigned char   hash[MAX_HASH_LEN];
    FILE*           fptr;
} MasterPassword;

int master_password_correct(MasterPassword master_pass);
int create_master_password(char master_password[MAX_STRING_LEN], char master_password_retype[MAX_STRING_LEN]);

int create_master_password_input();
int verify_master_password_input();

int auth_master_password();

#endif

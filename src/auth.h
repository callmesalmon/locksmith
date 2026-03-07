#ifndef AUTH_H
#define AUTH_H

#include <stdio.h>
#include "crypto.h"

typedef struct {
    char            password[MAX_STRING_LEN];
    unsigned char   hash[MAX_HASH_LEN];
    FILE*           fptr;
    int             num_tries;
} MasterPassword;

int master_password_correct(MasterPassword master_pass);
int create_master_password();
int check_master_password();
int auth_master_password();

#endif

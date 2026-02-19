#ifndef PASSWORD_H
#define PASSWORD_H

#include <string.h> // strcat

#include "commons.h"

#define locksmith_dir strcat(get_home_dir(), "/.locksmith/")
#define locksmith_passw_dir strcat(locksmith_dir, "passwords/")
#define locksmith_hash_key_dir strcat(locksmith_dir, "hashed_keys/")
#define locksmith_master_passw_file strcat(locksmith_dir, "master_password.txt")
#define get_locksmith_passw_dir_filepath(name) strcat(locksmith_passw_dir, strcat(name, ".txt"))
#define get_locksmith_hash_key_dir_filepath(name) strcat(locksmith_hash_key_dir, strcat(name, ".txt"))

int create_password(char name[], char password[]);
char *get_password(char name[]);
int delete_password(char name[]);
int list_passwords();
int verify_master_password(char* password, unsigned char* hash);

#endif

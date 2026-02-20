#ifndef PASSWORD_H
#define PASSWORD_H

#include <string.h> // strcat
#include <sodium.h>

#include "commons.h"

#define locksmith_dir strcat(get_home_dir(), "/.locksmith/")
#define locksmith_passw_dir strcat(locksmith_dir, "passwords/")
#define locksmith_hash_key_dir strcat(locksmith_dir, "hashed_keys/")
#define locksmith_key_file strcat(locksmith_dir, "key.txt")
#define get_locksmith_passw_dir_filepath(name) strcat(locksmith_passw_dir, strcat(name, ".txt"))
#define get_locksmith_hash_key_dir_filepath(name) strcat(locksmith_hash_key_dir, strcat(name, ".txt"))

int create_password(char name[], char password[], unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]);
char *get_password(char name[], unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]);
int delete_password(char name[]);
int list_passwords();
int get_key(unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]);

#endif

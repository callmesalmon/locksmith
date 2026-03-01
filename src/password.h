#ifndef PASSWORD_H
#define PASSWORD_H

#include <string.h> // strcat
#include <sodium.h>

#include "commons.h"

#define LOCKSMITH_DIR strcat(get_home_dir(), "/.locksmith/")
#define LOCKSMITH_PASSW_DIR strcat(LOCKSMITH_DIR, "passwords/")
#define LOCKSMITH_MASTER_PASSW_FILE strcat(LOCKSMITH_DIR, "master_password.hash")
#define LOCKSMITH_KEY_FILE strcat(LOCKSMITH_DIR, "locksmith.key")
#define GET_LOCKSMITH_PASSW_DIR_FILEPATH(name) strcat(LOCKSMITH_PASSW_DIR, strcat(name, ".enc"))

int create_password(char name[], char password[], unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]);
char *get_password(char name[], unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]);
int delete_password(char name[]);
int list_passwords();
int get_key(unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]);
int verify_master_password(char* password, unsigned char* hash);

#endif

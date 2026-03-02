#ifndef PASSWORD_H
#define PASSWORD_H

#include <string.h> // strcat
#include <sodium.h>

#include "commons.h"

// This is why we have macros, kid.

#define LOCKSMITH_DIR                   strcat(get_home_dir(), "/.locksmith/")          // ~/.locksmith

#define LOCKSMITH_PASSW_DIR             strcat(LOCKSMITH_DIR, "passwords/")             // ~/.locksmith/passwords
#define LOCKSMITH_MASTER_PASSW_FILE     strcat(LOCKSMITH_DIR, "master_password.hash")   // ~/.locksmith/master_password.hash
#define LOCKSMITH_KEY_FILE              strcat(LOCKSMITH_DIR, "locksmith.key")          // ~/.locksmith/locksmith.key

char *password_file(char *passname);
int password_exists(char *passname);
char *format_password_filename(char *site, char *user);

int create_password(char name[], char password[], unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]);
char *get_password(char name[], unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]);
int delete_password(char name[]);
int list_passwords();

int get_key(unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]);

#endif

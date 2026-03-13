#ifndef PASSWORD_H
#define PASSWORD_H

#include <string.h> // strcat
#include <sodium.h>

#include "commons.h"

// This is why we have macros, kid.

#define LOCKSMITH_DIR                   strcat(get_home_dir(), "/.locksmith/")          // ~/.locksmith

#define LOCKSMITH_PASSW_DIR             strcat(LOCKSMITH_DIR, "passwords/")             // ~/.locksmith/passwords
#define LOCKSMITH_BACKUP_DIR            strcat(LOCKSMITH_DIR, "backups/")               // ~/.locksmith/backups
#define LOCKSMITH_MASTER_PASSW_FILE     strcat(LOCKSMITH_DIR, "master_password.hash")   // ~/.locksmith/master_password.hash
#define LOCKSMITH_KEY_FILE              strcat(LOCKSMITH_DIR, "locksmith.key")          // ~/.locksmith/locksmith.key

char *password_file(char *passname);
char *backup_file(char *passname);
int password_exists(char *passname);

int create_password(char name[], char password[], unsigned char key[KEY_LEN]);
char *get_password(char name[], unsigned char key[KEY_LEN]);
int delete_password(char name[]);
int list_passwords();

int backup_password(char name[], char password[], unsigned char key[KEY_LEN]);
int recover_password(char name[]);
int clean_backups();

int get_key(unsigned char key[KEY_LEN]);

#endif

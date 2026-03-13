#ifndef PASSWORD_H
#define PASSWORD_H

#include <string.h> // strcat
#include <sodium.h>

#include "commons.h"
#include "crypto.h"

typedef struct {
    char title[MAX_STRING_LEN];
    char url[MAX_STRING_LEN];
    char username[MAX_STRING_LEN];
    char password[MAX_STRING_LEN];
} Password;

// This is why we have macros, kid.

#define LOCKSMITH_DIR                   strcat(get_home_dir(), "/.locksmith/")          // ~/.locksmith

#define LOCKSMITH_PASSW_DIR             strcat(LOCKSMITH_DIR, "passwords/")             // ~/.locksmith/passwords
#define LOCKSMITH_BACKUP_DIR            strcat(LOCKSMITH_DIR, "backups/")               // ~/.locksmith/backups
#define LOCKSMITH_MASTER_PASSW_FILE     strcat(LOCKSMITH_DIR, "master_password.hash")   // ~/.locksmith/master_password.hash
#define LOCKSMITH_KEY_FILE              strcat(LOCKSMITH_DIR, "locksmith.key")          // ~/.locksmith/locksmith.key

#define password_file(name)         strcat(password_dir(name), "/password.enc")
#define info_file(name)             strcat(password_dir(name), "/info.txt")
#define backup_password_file(name)  strcat(backup_dir(name), "/password.enc")
#define backup_info_file(name)      strcat(backup_dir(name), "/info.txt")

char *password_dir(char *path);
char *backup_dir(char *path);
int password_exists(char *path);

// setting: 'p': password, 'b': backup
int create_password_file(Password pass, unsigned char key[KEY_LEN], char setting);
int create_info_file(Password pass, char setting);

int create_password(Password pass, unsigned char key[KEY_LEN]);

Password get_password(char name[], unsigned char key[KEY_LEN]);
int delete_password(char name[]);
int list_passwords();

int backup_password(Password pass);
int recover_password(char name[]);
int clean_backups();

int get_key(unsigned char key[KEY_LEN]);

#endif

#include <dirent.h>
#include <stdio.h>

#include "password.h"
#include "crypto.h"
#include "cli_msg.h"

/**** Password utility functions ****/

char *password_file(char *passname) {
    static char full_filename[MAX_STRING_LEN];
    snprintf(full_filename, MAX_STRING_LEN, "%s%s.enc", LOCKSMITH_PASSW_DIR, passname);

    return full_filename;
}

char *backup_file(char *passname) {
    static char full_filename[MAX_STRING_LEN];
    snprintf(full_filename, MAX_STRING_LEN, "%s%s.bak", LOCKSMITH_BACKUP_DIR, passname);

    return full_filename;
}

int password_exists(char *passname) {
    char *full_filename = password_file(passname);

    if (fexists(full_filename)) {
        return 1;
    }
    return 0;
}

char *format_password_filename(char *site, char *user) {
    static char full_filename[MAX_STRING_LEN];
    snprintf(full_filename, sizeof(full_filename), "%s:%s", site, user);

    return full_filename;
}

/**** Main password interface ****/

int create_password(char name[], char password[], unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]) {
    char *fname = password_file(name);

    FILE *fptr;

    fptr = fopen(fname, "w");
    if (fptr == NULL) {
        cli_error_die(-1, "Couldn't create password. Failed to write to file '%s'.\n", fname);
    }
    fprintf(fptr, "%s", password);
    fclose(fptr); 

    encrypt(fname, password, key);

    return 0;
}

char *get_password(char name[], unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]) {
    char *fname = password_file(name);
    unsigned char *buf = decrypt(fname, key);

    encrypt(fname, (char *)buf, key);

    return (char *)buf;
}

int delete_password(char name[]) {
    char *fname = password_file(name);
    remove(fname);

    return 0;
}

int list_passwords() {
    struct dirent *de;

    DIR *dir = opendir(LOCKSMITH_PASSW_DIR);

    if (dir == NULL) {
        cli_error_die(-1, "Couldn't list passwords.");
    }

    while ((de = readdir(dir)) != NULL) {
        char *fname = de->d_name;
        strip_ext(fname);

        // fname != "." is for the "." (cwd) and ".." ""files""
        if (fname != NULL && strcmp(fname, ".") != 0) printf("%s\n", fname);
    }

    closedir(dir);    

    return 0;
}

/**** Backups ****/

int backup_password(char name[], char password[], unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]) {
    char *fname = backup_file(name);

    FILE *fptr;

    fptr = fopen(fname, "w");
    if (fptr == NULL) {
        cli_error_die(-1, "Couldn't backup password. Failed to write to file '%s'.\n", fname);
    }
    fprintf(fptr, "%s", password);
    fclose(fptr); 

    encrypt(fname, password, key);

    return 0;
}

int clean_backups() {
    struct dirent *de;

    DIR *dir = opendir(LOCKSMITH_BACKUP_DIR);

    if (dir == NULL) {
        cli_error_die(-1, "Couldn't clean backups.");
    }

    while ((de = readdir(dir)) != NULL) {
        char *fname = de->d_name;
        strip_ext(fname);

        int not_dirfile = ((strcmp(fname, ".") != 0) && (strcmp(fname, "..") != 0));
        if (!password_exists(fname) && not_dirfile) {
            remove(backup_file(fname));
        }
    }

    closedir(dir);

    return 0;
}

/**** Key-related ****/

int get_key(unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]) {
    FILE *key_fd = fopen(LOCKSMITH_KEY_FILE, "r");
    if (key_fd == NULL) {
        cli_error_die(-1, "Couldn't get key.\n");
        exit(2);
    }

    fread(&key, crypto_secretstream_xchacha20poly1305_KEYBYTES, 1, key_fd);

    return 0;
}

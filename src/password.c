#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "password.h"
#include "crypto.h"

char *password_file(char *passname) {
    static char full_filename[MAX_STRING_LEN];
    snprintf(full_filename, MAX_STRING_LEN, "%s%s.enc", LOCKSMITH_PASSW_DIR, passname);

    return full_filename;
}

int password_exists(char *passname) {
    static char full_filename[MAX_STRING_LEN];
    snprintf(full_filename, MAX_STRING_LEN, "%s%s.enc", LOCKSMITH_PASSW_DIR, passname);

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

int create_password(char name[], char password[], unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]) {
    char *fname = password_file(name);

    FILE *fptr;

    fptr = fopen(fname, "w");
    if (fptr == NULL) {
        die("Couldn't create password. Failed to write to file.");
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
        die("Couldn't list passwords.");
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

int get_key(unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]) {
    FILE *key_fd = fopen(LOCKSMITH_KEY_FILE, "r");
    if (key_fd == NULL) {
        die("ERROR: Couldn't get key.\n");
        exit(2);
    }

    fread(&key, crypto_secretstream_xchacha20poly1305_KEYBYTES, 1, key_fd);

    return 0;
}

#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "password.h"
#include "crypto.h"

int create_password(char name[], char password[], unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]) {
    char *fname = get_locksmith_passw_dir_filepath(name);

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
    char *fname = get_locksmith_passw_dir_filepath(name);

    unsigned char *buf = decrypt(fname, key);

    encrypt(fname, (char *)buf, key);

    return (char *)buf;
}

int delete_password(char name[]) {
    char *fname = get_locksmith_passw_dir_filepath(name);
    remove(fname);

    return 0;
}

int list_passwords() {
    struct dirent *de;

    DIR *dir = opendir(locksmith_passw_dir);

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
    FILE *key_fd = fopen(locksmith_key_file, "r");
    if (key_fd == NULL) {
        errno = 2;
        exit(2);
    }

    fread(&key, crypto_secretstream_xchacha20poly1305_KEYBYTES, 1, key_fd);

    return 0;
}

int verify_master_password(char* password, unsigned char* hash) {
    unsigned char password_hash[MAX_HASH_LEN];
    hash_password(password, password_hash);

    for (int i = 0; password[i] != '\0'; i++) {
        if (password_hash[i] != hash[i])
            return 0;
    }

    return 1;
}

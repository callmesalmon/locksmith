#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>

#include "password.h"
#include "crypto.h"

int create_password(char name[], char password[]) {
    char *fname = get_locksmith_passw_dir_filepath(name);

    FILE *fptr;

    fptr = fopen(fname, "w");
    if (fptr == NULL) {
        die("Couldn't create password. Failed to write to file.");
        return 200;
    }
    fprintf(fptr, "%s", password);
    fclose(fptr); 

    encrypt_decrypt(fname);

    return 0;
}

char *get_password(char name[]) {
    char *fname = get_locksmith_passw_dir_filepath(name);
    IFVERBOSE("get_locksmith_passw_dir_filepath: %s\n", fname);

    FILE *fptr;

    encrypt_decrypt(fname);
    fptr = fopen(fname, "r");
    if (fptr == NULL) {
        die("Couldn't get password. Failed to read file.");
        return NULL;
    }

    fseek(fptr, 0, SEEK_END);
    long filesize = ftell(fptr);
    IFVERBOSE("File size: %ld bytes\n", filesize);
    rewind(fptr);

    static char fcontent[100]; // Return values NEED to be global, due to how the stack works.
    fgets(fcontent, 100, fptr);

    IFVERBOSE("Read value: \"%s\"\n", fcontent);

    fclose(fptr);
    encrypt_decrypt(fname);

    return fcontent;
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
        return 200;
    }

    while ((de = readdir(dir)) != NULL) {
        char *pname = strtok(de->d_name, ".");
        if (pname != NULL) printf("%s\n", pname);
    }

    closedir(dir);    

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

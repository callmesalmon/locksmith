#include <dirent.h>
#include <stdio.h>
#include <unistd.h>

#include "password.h"
#include "commons.h"
#include "crypto.h"
#include "cli_msg.h"

/**** Password utility functions ****/

char *password_dir(char *path) {
    static char full_filename[MAX_STRING_LEN];
    snprintf(full_filename, MAX_STRING_LEN, "%s%s", LOCKSMITH_PASSW_DIR, path);

    return full_filename;
}

char *backup_dir(char *path) {
    static char full_filename[MAX_STRING_LEN];
    snprintf(full_filename, MAX_STRING_LEN, "%s%s", LOCKSMITH_BACKUP_DIR, path);

    return full_filename;
}

int password_exists(char *path) {
    char *full_dirname = password_dir(path);

    if (fexists(full_dirname)) {
        return 1;
    }
    return 0;
}

/**** Main password interface ****/

int create_password_file(Password pass, unsigned char key[KEY_LEN], char *fname) {
    int result = file_write(fname, pass.password);
    if (result == -1) {
        return result;
    }

    encrypt(fname, pass.password, key);

    return 0;
}

int create_info_file(Password pass, char *fname) {
    char buf[MAX_STRING_LEN * 2];
    snprintf(buf, MAX_STRING_LEN * 2, "%s\n%s", pass.url, pass.username);

    int result = file_write(fname, buf);
    if (result == -1) {
        return result;
    }

    return 0;
}

int create_password(Password pass, unsigned char key[KEY_LEN]) {
    int result;

    result = mkdir_if_not_exist(password_dir(pass.title));
    if (result == -1) {
        return -1;
    }

    result = create_password_file(pass, key, password_file(pass.title));
    if (result == -1) {
        return cli_error("Couldn't create file '%s'!\n", password_file(pass.title));
    }

    result = create_info_file(pass, info_file(pass.title));
    if (result == -1) {
        return cli_error("Couldn't create file '%s'!\n", info_file(pass.title));
    }

    return 0;
}

char *get_password_from_file(char name[], unsigned char key[KEY_LEN]) {
    char *buf = decrypt(name, key);

    encrypt(name, buf, key);

    return buf;
}

char** get_info_from_file(char name[]) {
    FILE *fptr = fopen(name, "r");

    if (fptr == NULL) {
        return NULL;
    }

    static char *data[2];
    for (int i = 0; i < 2; i++) {
        data[i] = malloc(MAX_STRING_LEN);
        if (data[i] == NULL) {
            fclose(fptr);
            return NULL;
        }
    }

    int line = 0;
    while (line < 2 && fgets(data[line], MAX_STRING_LEN, fptr) != NULL) {
        line++;
    }

    fclose(fptr);
    return data;
}

Password get_password(char name[], unsigned char key[KEY_LEN]) {
    Password pass;

    char *passfile = password_file(name);

    // Like 60% sure this is safe anyways
    strncpy(pass.password, get_password_from_file(passfile, key), MAX_STRING_LEN);

    // Do the info part
    char *infofile = info_file(name);

    char **data = get_info_from_file(infofile);

    if (data == NULL) {
        cli_error("Couldn't get password info from '%s'!", infofile);
        return PASSWORD_ERROR;
    }

    strncpy(pass.url, data[0], MAX_STRING_LEN);
    strncpy(pass.username, data[1], MAX_STRING_LEN);

    for (int i = 0; i < 2; i++) {
        free(data[i]);
    }

    return pass;
}

int delete_password(char name[]) {
    remove(password_file(name));
    remove(info_file(name));

    int result = rmdir(password_dir(name));
    if (result != 0) {
        return -1;
    }

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
        
        int special = (!strcmp(fname, ".") || !strcmp(fname, ".."));

        // fname != "." is for the "." (cwd) and ".." ""files""
        if (fname != NULL && !special) printf("%s\n", fname);
    }

    closedir(dir);

    return 0;
}

/**** Backups ****/

int backup_password(Password pass) {
    int result;

    result = mkdir_if_not_exist(backup_dir(pass.title));
    if (result == -1) {
        return -1;
    }

    char buf[MAX_ENC_LEN];
    FILE *fptr = fopen(password_file(pass.title), "rb");
    fread(buf, 1, MAX_ENC_LEN, fptr);

    result = file_write(backup_password_file(pass.title), buf);
    if (result == -1) {
        return cli_error("Couldn't create file '%s'!\n", backup_password_file(pass.title));
    }

    result = create_info_file(pass, backup_info_file(pass.title));
    if (result == -1) {
        return cli_error("Couldn't create file '%s'!\n", backup_info_file(pass.title));
    }

    return 0;
}

int recover_password(char name[]) {
    mkdir_if_not_exist(password_dir(name));

    file_copy(password_file(name), backup_password_file(name));
    file_copy(info_file(name), backup_info_file(name));

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

        struct dirent *inner_de;
        DIR *inner_dir = opendir(strcat(LOCKSMITH_BACKUP_DIR, fname));

        if (dir == NULL)
            cli_error_die(-1, "Couldn't clean backups.");

        while ((inner_de = readdir(inner_dir)) != NULL) {
            if (fexists(password_dir(fname)))
                break;

            char *inner_fname = inner_de->d_name;
            
            int special = (!strcmp(inner_fname, ".") || !strcmp(inner_fname, "."));

            if (!special)
                remove(inner_fname);
        }

        closedir(inner_dir);
    }

    closedir(dir);

    return 0;
}

/**** Key-related ****/

int get_key(unsigned char key[KEY_LEN]) {
    FILE *key_fd = fopen(LOCKSMITH_KEY_FILE, "r");
    if (key_fd == NULL) {
        cli_error_die(-1, "Couldn't get key.\n");
        exit(2);
    }

    fread(&key, KEY_LEN, 1, key_fd);

    return 0;
}

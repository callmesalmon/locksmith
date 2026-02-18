//    Copyright (C) 2026, Salmon
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <stdarg.h>

#include "commons.h"
#include "crypto.h"

#define locksmith_title \
"88                                88                                     88         88         \n" \
"88                                88                                     \"\"   ,d    88         \n" \
"88                                88                                          88    88         \n" \
"88          ,adPPYba,   ,adPPYba, 88   ,d8  ,adPPYba, 88,dPYba,,adPYba,  88 MM88MMM 88,dPPYba, \n" \
"88         a8\"     \"8a a8\"     \"\" 88 ,a8\"   I8[    \"\" 88P'   \"88\"    \"8a 88   88    88P'    \"8a\n" \
"88         8b       d8 8b         8888[      `\"Y8ba,  88      88      88 88   88    88       88\n" \
"88         \"8a,   ,a8\" \"8a,   ,aa 88`\"Yba,  aa    ]8I 88      88      88 88   88,   88       88\n" \
"88888888888 `\"YbbdP\"'   \"Ybbd8\"' 88   `Y8a `\"YbbdP\"' 88      88      88 88   \"Y888 88       88\n\n" \

#define locksmith_dir strcat(get_home_dir(), "/.locksmith/")
#define locksmith_passw_dir strcat(locksmith_dir, "passwords/")
#define locksmith_hash_key_dir strcat(locksmith_dir, "hashed_keys/")
#define locksmith_master_passw_file strcat(locksmith_dir, "master_password.txt")
#define get_locksmith_passw_dir_filepath(name) strcat(locksmith_passw_dir, strcat(name, ".txt"))
#define get_locksmith_hash_key_dir_filepath(name) strcat(locksmith_hash_key_dir, strcat(name, ".txt"))

#define format_password_filename(site, user) strcat(strcat(site, ":"), user)

// TODO: move this somewhere
int exit_cmd;

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

int cmd_create_password() {
    char site_name[MAX_STRING_LEN], user_name[MAX_STRING_LEN], password[MAX_STRING_LEN];

    printf("Site:\n> ");
    get_input(site_name);

    printf("Username:\n> ");
    get_input(user_name);

    printf("Password:\n> ");
    get_input(password);

    create_password(format_password_filename(site_name, user_name), password);

    return 0;
}

int cmd_get_password() {
    char pass_name[MAX_STRING_LEN];

    printf("What password do you want to get?\n"
            "List of passwords:\n");
    list_passwords();
    printf("> ");
    get_input(pass_name);

    printf("Password: %s\n", get_password(pass_name));

    return 0;
}

int cmd_delete_password() {
    char pass_name[MAX_STRING_LEN];

    printf("What password do you want to delete?\n"
            "List of passwords:\n");
    list_passwords();
    printf("> ");
    get_input(pass_name);

    delete_password(pass_name);
    
    return 0;
}

int verify_master_password_interface() {
    unsigned char master_password_hash[MAX_HASH_LEN];
    char master_password[MAX_STRING_LEN];

    FILE* fptr = fopen(locksmith_master_passw_file, "rb");
    if (fptr == NULL) {
        printf("Create master password:\n> ");
        get_input(master_password);
        hash_password(master_password, master_password_hash);

        fptr = fopen(locksmith_master_passw_file, "wb");
        fwrite(master_password_hash, 1, MAX_HASH_LEN, fptr);
        fclose(fptr);
    }
    else {
        unsigned char master_pass_actual_hash[MAX_HASH_LEN];
        fread(master_pass_actual_hash, 1, MAX_HASH_LEN, fptr);

        int password_verified = 0;
        while (!password_verified) {
            printf("Enter master password (0 to exit):\n> ");
            get_input(master_password);

            if (!strcmp(master_password, "0")) exit(0);
            if (verify_master_password(master_password, master_pass_actual_hash)) break;

            printf("Wrong password!\n");
        }

        fclose(fptr);
    }

    return 0;
}

int cmd_interface() {
    printf("\nEnter a command:\n"
           "[1]: Create password\n"
           "[2]: Get password\n"
           "[3]: Delete password\n"
           "[4]: Exit program\n");

    int command;
    printf("> ");
    get_iinput(&command);

    char site_name[MAX_STRING_LEN];
    char user_name[MAX_STRING_LEN];
    char password[MAX_STRING_LEN];

    switch (command) {
        case 1:
            cmd_create_password();
            break;
        case 2:
            cmd_get_password();
            break;
        case 3:
            cmd_delete_password();
            break;
        case 4:
            printf("Exiting...\n");
            exit_cmd = 1;
            break;
        default:
            printf("Invalid command! Exiting...\n");
            exit_cmd = 1;
    }

    return 0;
}

int main(int argc, char **argv) {
    safe_srand();

    mkdirifnotexist(locksmith_dir);
    mkdirifnotexist(locksmith_passw_dir);

    printf(locksmith_title);

    verify_master_password_interface();

    exit_cmd = 0;
    while (!exit_cmd) {
        cmd_interface();
    }

    return 0;
}

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
#include <dirent.h>

#define VERBOSE 0 // Change to 1 to turn on debug/verbose mode.
#define IFVERBOSE(...) if (VERBOSE == 1) printf(__VA_ARGS__);

char *get_home_dir() {
    return getpwuid(getuid())->pw_dir;
}

// ~/.locksmith/
char *get_locksmith_dir() {
    return strcat(get_home_dir(), "/.locksmith/");
}

char *get_locksmith_dir_filepath(char *name) {
    return strcat(get_locksmith_dir(), strcat(name, ".txt")); // It's ugly, I know.
}

int directory_exists(char *dirname) {
    struct stat st;
    if (stat(dirname, &st) == 0) {
        return 1;
    }
    return 0;
}

int rand_range(int min, int max) {
    unsigned int seed = time(0);
    return rand_r(&seed) % (max - min + 1) + min;
}

int encrypt(char fname[], int key) {
    IFVERBOSE("Encrypting %s...\n", fname);

    FILE *fps, *fpt;
    char ch;
    
    fps = fopen(fname, "r");
    if (fps == NULL) {
        printf("ERROR: Couldn't encrypt password. Failed to open file.\n");
        return 200;
    }
    fpt = fopen(".temp.txt", "w");
    if (fpt == NULL) {
        printf("ERROR: Couldn't encrypt password. Failed to open tempfile.\n");
        return 200;
    }
    
    ch = fgetc(fps);
    while (ch != EOF) {
        ch = ch + key;
        fputc(ch, fpt);
        ch = fgetc(fps);
    }
    
    fclose(fps);
    fclose(fpt);
    
    fps = fopen(fname, "w");
    if (fps == NULL) {
        printf("ERROR: Couldn't encrypt password. Failed to open file.\n");
        return 200;
    }
    fpt = fopen(".temp.txt", "r");
    if (fpt == NULL) {
        printf("ERROR: Couldn't encrypt password. Failed to open tempfile.\n");
        return 200;
    }
 
    ch = fgetc(fpt);
    while(ch != EOF) {
        ch = fputc(ch, fps);
        ch = fgetc(fpt);
    }
    
    fclose(fps);
    fclose(fpt);

    remove(".temp.txt");

    return 0;
}

int decrypt(char fname[], int key) {
    IFVERBOSE("Decrypting %s...\n", fname);

    FILE *fps, *fpt;
    char ch;

    fps = fopen(fname, "r");
    if (fps == NULL) {
        printf("ERROR: Couldn't decrypt password. Failed to open file.\n");
        return 200;
    }
    fpt = fopen(".temp.txt", "w");
    if (fpt == NULL) {
        printf("ERROR: Couldn't decrypt password. Failed to open tempfile.\n");
        return 200;
    }

    IFVERBOSE("Decrypted string: ");
    ch = fgetc(fps);
    while(ch != EOF) {
        ch = ch - key;
        fputc(ch, fpt);
        IFVERBOSE("%c", ch);
        ch = fgetc(fps);
    }
    IFVERBOSE("\n");

    fclose(fps);
    fclose(fpt);

    fps = fopen(fname, "w");
    if (fps == NULL) {
        printf("ERROR: Couldn't decrypt password. Failed to open file.\n");
        return 200;
    }
    fpt = fopen(".temp.txt", "r");
    if (fpt == NULL) {
        printf("ERROR: Couldn't decrypt password. Failed to open tempfile.\n");
        return 200;
    }

    ch = fgetc(fpt);
    while(ch != EOF) {
        fputc(ch, fps);
        ch = fgetc(fpt);
    }

    fclose(fps);
    fclose(fpt);
    
    remove(".temp.txt");

    return 0;
}

int create_password(char name[], char password[], int key) {
    char *fname = get_locksmith_dir_filepath(name);

    FILE *fptr;

    fptr = fopen(fname, "w");
    if (fptr == NULL) {
        printf("ERROR: Couldn't create password. Failed to write to file.\n");
        return 200;
    }
    fprintf(fptr, "%s", password);
    fclose(fptr); 

    encrypt(fname, key);

    return 0;
}

char *get_password(char name[], int key) {
    char *fname = get_locksmith_dir_filepath(name);
    IFVERBOSE("get_locksmith_dir_filepath: %s\n", fname);

    FILE *fptr;

    decrypt(fname, key);
    fptr = fopen(fname, "r");
    if (fptr == NULL) {
        printf("ERROR: Couldn't get password. Failed to read file.\n");
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
    encrypt(fname, key);

    return fcontent;
}

int delete_password(char name[]) {
    char *fname = get_locksmith_dir_filepath(name);
    remove(fname);

    return 0;
}

int list_passwords() {
    struct dirent *de;

    DIR *dir = opendir(get_locksmith_dir());

    if (dir == NULL) {
        printf("ERROR: Couldn't list passwords.\n");
        return 200;
    }

    while ((de = readdir(dir)) != NULL) {
        char *pname = strtok(de->d_name, ".");
        if (pname != NULL) printf("%s\n", pname);
    }

    closedir(dir);    

    return 0;
}

int main(int argc, char **argv) {
    srand(time(NULL));

    if (directory_exists(get_locksmith_dir()) == 0) {
        mkdir(get_locksmith_dir(), 0755);
    }

    printf(                                                                                              
"88                                88                                     88         88         \n"  
"88                                88                                     \"\"   ,d    88         \n"  
"88                                88                                          88    88         \n" 
"88          ,adPPYba,   ,adPPYba, 88   ,d8  ,adPPYba, 88,dPYba,,adPYba,  88 MM88MMM 88,dPPYba, \n" 
"88         a8\"     \"8a a8\"     \"\" 88 ,a8\"   I8[    \"\" 88P'   \"88\"    \"8a 88   88    88P'    \"8a\n" 
"88         8b       d8 8b         8888[      `\"Y8ba,  88      88      88 88   88    88       88\n" 
"88         \"8a,   ,a8\" \"8a,   ,aa 88`\"Yba,  aa    ]8I 88      88      88 88   88,   88       88\n" 
"88888888888 `\"YbbdP\"'   \"Ybbd8\"' 88   `Y8a `\"YbbdP\"' 88      88      88 88   \"Y888 88       88\n\n");
    printf("Enter a command:\n"
           "[1]: Create password\n"
           "[2]: Get password\n"
           "[3]: Delete password\n");

    int command;
    printf(">");
    scanf("%d", &command);


    char service[100];
    char password[100];
    int key;

    switch (command) {
        case 1:
            printf("What service is your password related to? (ex. Github, Google Drive)\n>");
            scanf("%s", service);

            printf("Enter password.\n>");
            scanf("%s", password);

            printf("Enter key. (A number preferably 100-300, preferably global,"
                   "that is used to encrypt/decrypt passwords)\n>");
            scanf("%d", &key);

            create_password(service, password, key);

            break;

        case 2:
            printf("What password do you want to get?\n"
                   "List of passwords:\n");
            list_passwords();
            printf(">");
            scanf("%s", service);

            printf("Enter key.\n>");
            scanf("%d", &key);

            printf("Password: %s\n", get_password(service, key));

            break;

        case 3:
            printf("What password do you want to delete?\n"
                   "List of passwords:\n");
            list_passwords();
            printf(">");
            scanf("%s", service);

            delete_password(service);

            break;

        default:
            printf("Invalid command! Exiting...\n");
    }

    return 0;
}

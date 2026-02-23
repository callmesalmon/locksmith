#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/time.h>
#include <stdarg.h>
#include <string.h>

#include "cli.h"
#include "commons.h"
#include "crypto.h"
#include "password.h"

#define LIST_ITEM_NUMBER(n) "[" CYAN n DEFAULT_COLOR "] "
#define FORMAT_PASSWORD_FILENAME(site, user) strcat(strcat(site, ":"), user)

#define LOCKSMITH_PROMPT1 "\n[" RED "locksmith" DEFAULT_COLOR "]$ "
#define LOCKSMITH_PROMPT2 "\n> "

#define QUESTION_COLOR      UNDERLINE_CYAN
#define GETPASS_COLOR       UNDERLINE_RED
#define ENTER_CMD_COLOR     UNDERLINE_RED

static unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];

int cli_init() {
    get_key(key);

    return 0;
}

int cmd_create_password() {
    char site_name[MAX_STRING_LEN], user_name[MAX_STRING_LEN], password[MAX_STRING_LEN];

    printf_color(QUESTION_COLOR, "\nSite:");
    printf(LOCKSMITH_PROMPT2);
    safe_scanf(MAX_STRING_LEN, "%s", site_name);

    printf_color(QUESTION_COLOR, "\nUsername:");
    printf(LOCKSMITH_PROMPT2);
    safe_scanf(MAX_STRING_LEN, "%s", user_name);

    printf_color(QUESTION_COLOR, "\nPassword:");
    printf(LOCKSMITH_PROMPT2);
    safe_scanf(MAX_STRING_LEN, "%s", password);

    create_password(FORMAT_PASSWORD_FILENAME(site_name, user_name), password, key);

    return 0;
}

int cmd_get_password() {
    char pass_name[MAX_STRING_LEN];

    printf_color(QUESTION_COLOR, "\nWhat password do you want to get?\n");
    list_passwords();
    printf(LOCKSMITH_PROMPT2);
    safe_scanf(MAX_STRING_LEN, "%s", pass_name);

    printf_color(GETPASS_COLOR, "Password:");
    printf(" %s\n", get_password(pass_name, key));

    return 0;
}

int cmd_delete_password() {
    char pass_name[MAX_STRING_LEN];

    printf_color(QUESTION_COLOR, "\nWhat password do you want to delete?\n");
    list_passwords();
    printf(LOCKSMITH_PROMPT2);
    safe_scanf(MAX_STRING_LEN, "%s", pass_name);

    delete_password(pass_name);
    
    return 0;
}

int cmd_interface(int *exit) {
    printf_color(ENTER_CMD_COLOR, "\nEnter a command:\n");
    printf(
           LIST_ITEM_NUMBER("1") "Create password\n"
           LIST_ITEM_NUMBER("2") "Get password\n"
           LIST_ITEM_NUMBER("3") "Delete password\n"
           LIST_ITEM_NUMBER("4") "Exit program\n");

    int command;
    int command_len = sizeof(int);
    printf(LOCKSMITH_PROMPT1);
    safe_scanf(command_len, "%d", &command);

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
            *exit = 1;
            break;
        default:
            printf("Invalid command! Exiting...\n");
            *exit = 1;
    }

    return 0;
}

int verify_master_password_interface() {
    unsigned char master_password_hash[MAX_HASH_LEN];
    char master_password[MAX_STRING_LEN];

    FILE* fptr = fopen(LOCKSMITH_MASTER_PASSW_FILE, "rb");
    if (fptr == NULL) {
        printf("Create master password:\n> ");
        safe_scanf(MAX_STRING_LEN, "%s", master_password);
        hash_password(master_password, master_password_hash);

        fptr = fopen(LOCKSMITH_MASTER_PASSW_FILE, "wb");
        fwrite(master_password_hash, 1, MAX_HASH_LEN, fptr);
        fclose(fptr);
    }
    else {
        unsigned char master_pass_actual_hash[MAX_HASH_LEN];
        fread(master_pass_actual_hash, 1, MAX_HASH_LEN, fptr);

        int password_verified = 0;
        while (!password_verified) {
            printf("Enter master password (0 to exit):\n> ");
            safe_scanf(MAX_STRING_LEN, "%s", master_password);

            if (!strcmp(master_password, "0")) exit(0);
            if (verify_master_password(master_password, master_pass_actual_hash)) break;

            printf("Wrong password!\n\n");
        }

        fclose(fptr);
    }

    return 0;
}

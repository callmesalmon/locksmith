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

// e.g:
// printf(LIST_ITEM_NUMBER(1) LIST_ITEM_STRING("do thing\n"))
// -> [\CYAN1\DEFAULT] \CYANdo thing\n\DEFAULT
#define LIST_ITEM_NUMBER(n) "[" CYAN n DEFAULT_COLOR "] "
#define LIST_ITEM_STRING(s) CYAN s DEFAULT_COLOR

#define LOCKSMITH_PROMPT1 "\n[" RED "locksmith" DEFAULT_COLOR "]$ "
#define LOCKSMITH_PROMPT2 "\n> "

static unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];

int cli_init() {
    get_key(key);

    return 0;
}

char *format_password_filename(char *site, char *user) {
    char *local_site = site;
    char *local_user = user;
    static char full_filename[MAX_STRING_LEN];

    snprintf(full_filename, sizeof(full_filename), "%s:%s", site, user);

    return full_filename;
}

// We need to check if passname + .txt exists
int passname_handler(char *passname) {
    static char full_filename[MAX_STRING_LEN];
    snprintf(full_filename, MAX_STRING_LEN, "%s%s.txt", LOCKSMITH_PASSW_DIR, passname);

    if (fexists(full_filename)) {
        return 1;
    }
    return 0;
}

int cmd_create_password() {
    char site_name[MAX_STRING_LEN], user_name[MAX_STRING_LEN], password[MAX_STRING_LEN];

    printf_color(UNDERLINE_CYAN, "\nSite:");
    printf(LOCKSMITH_PROMPT2);
    safe_scanf(MAX_STRING_LEN, "%s", site_name);

    printf_color(UNDERLINE_CYAN, "\nUsername:");
    printf(LOCKSMITH_PROMPT2);
    safe_scanf(MAX_STRING_LEN, "%s", user_name);

    printf_color(UNDERLINE_CYAN, "\nPassword:");
    printf(LOCKSMITH_PROMPT2);
    safe_scanf(MAX_STRING_LEN, "%s", password);

    // This is not a good implementation. In reality we should just remove
    // the automatic prepending of LOCKSMITH_DIR* to all filenames.
    char *full_filename = format_password_filename(site_name, user_name);
    char full_filename_with_dir[MAX_STRING_LEN];
    snprintf(full_filename_with_dir, MAX_STRING_LEN, "%s%s.txt", LOCKSMITH_PASSW_DIR, full_filename);
    
    if (fexists(full_filename_with_dir)) {
        printf_color(YELLOW, "WARNING: Password file already exists!\n");
        printf_color(YELLOW, "Are you sure you want to overwrite this password? [y/N] ");

        char overwrite_pass[MAX_STRING_LEN];
        safe_scanf(MAX_STRING_LEN, "%s", overwrite_pass);
        
        if (strcmp(overwrite_pass, "y") != 0) return 0;
    }

    create_password(full_filename, password, key);

    return 0;
}

int cmd_get_password() {
    char pass_name[MAX_STRING_LEN];

    printf_color(UNDERLINE_CYAN, "\nWhat password do you want to get?\n");
    list_passwords();
    printf(LOCKSMITH_PROMPT2);
    safe_scanf(MAX_STRING_LEN, "%s", pass_name);

    if (!passname_handler(pass_name)) {
        printf_color(YELLOW, "WARNING: Password file not accessible!\n");
        printf_color(YELLOW, "Returning to command interface...\n");
        return 1;
    }

    printf_color(UNDERLINE_RED, "Password:");
    printf(" %s\n", get_password(pass_name, key));

    return 0;
}

int cmd_delete_password() {
    char pass_name[MAX_STRING_LEN];

    printf_color(UNDERLINE_CYAN, "\nWhat password do you want to delete?\n");
    list_passwords();
    printf(LOCKSMITH_PROMPT2);
    safe_scanf(MAX_STRING_LEN, "%s", pass_name);

    if (!passname_handler(pass_name)) {
        printf_color(YELLOW, "WARNING: Password file not accessible!\n");
        printf_color(YELLOW, "Returning to command interface...\n");
        return 1;
    }

    delete_password(pass_name);
    
    return 0;
}

int cmd_interface(int *exit) {
    printf_color(UNDERLINE_RED, "\nEnter a command:\n");
    printf(
           LIST_ITEM_NUMBER("1") LIST_ITEM_STRING("Create password\n")
           LIST_ITEM_NUMBER("2") LIST_ITEM_STRING("Get password\n")
           LIST_ITEM_NUMBER("3") LIST_ITEM_STRING("Delete password\n")
           LIST_ITEM_NUMBER("4") LIST_ITEM_STRING("Exit program\n"));

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

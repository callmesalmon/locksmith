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

#define LOCKSMITH_PROMPT1 "\n[" RED "locksmith" DEFAULT_COLOR "]$ "

#define LIST_OF_COMMANDS          \
"new - create new password\n"     \
"get - get password\n"            \
"del - delete password\n"      \
"exit - exit command interface\n" \
"help - show this text\n"         \

static unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];

typedef enum {
    CMD_NEW,
    CMD_GET,
    CMD_DEL,
    CMD_EXIT,
    CMD_HELP,

    INVALID,
} CommandList;

CommandList get_cmd_value(char str[MAX_STRING_LEN]) {
    if (!strcmp(str, "new"))  return CMD_NEW;
    if (!strcmp(str, "get"))  return CMD_GET;
    if (!strcmp(str, "del"))  return CMD_DEL;
    if (!strcmp(str, "exit")) return CMD_EXIT;
    if (!strcmp(str, "help")) return CMD_HELP;
    return INVALID;
}

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

int cli_error(char *message) {
    printf_color(RED, "ERROR: %s", message);
    return 0;
}


int cli_warning(char *message) {
    printf_color(YELLOW, "WARNING: %s", message);
    return 0;
}

int cmd_create_password() {
    char site_name[MAX_STRING_LEN], user_name[MAX_STRING_LEN], password[MAX_STRING_LEN];

    printf("Sitename: ");
    safe_scanf(MAX_STRING_LEN, "%s", site_name);

    printf("Username: ");
    safe_scanf(MAX_STRING_LEN, "%s", user_name);

    printf("Password: ");
    safe_scanf(MAX_STRING_LEN, "%s", password);

    // This is not a good implementation. In reality we should just remove
    // the automatic prepending of LOCKSMITH_DIR* to all filenames.
    char *full_filename = format_password_filename(site_name, user_name);
    char full_filename_with_dir[MAX_STRING_LEN];
    snprintf(full_filename_with_dir, MAX_STRING_LEN, "%s%s.txt", LOCKSMITH_PASSW_DIR, full_filename);
    
    if (fexists(full_filename_with_dir)) {
        cli_warning("Password file already exists!\n"
                    "Are you sure you want to overwrite this password? [y/N] ");

        char overwrite_pass[MAX_STRING_LEN];
        safe_scanf(MAX_STRING_LEN, "%s", overwrite_pass);
        
        if (strcmp(overwrite_pass, "y") != 0) return 0;
    }

    create_password(full_filename, password, key);

    return 0;
}

int cmd_get_password() {
    char pass_name[MAX_STRING_LEN];

    printf("\nWhat password do you want to get?\n");
    list_passwords();
    printf(">");
    safe_scanf(MAX_STRING_LEN, "%s", pass_name);

    if (!passname_handler(pass_name)) {
        cli_error("Password file not accessible!\n"
                  "Returning to command interface...\n");
        return 1;
    }

    printf_color(UNDERLINE_RED, "Password:");
    printf(" %s\n", get_password(pass_name, key));

    return 0;
}

int cmd_delete_password() {
    char pass_name[MAX_STRING_LEN];

    printf("\nWhat password do you want to delete?\n");
    list_passwords();
    printf(">");
    safe_scanf(MAX_STRING_LEN, "%s", pass_name);

    if (!passname_handler(pass_name)) {
        cli_error("Password file not accessible!\n"
                  "Returning to command interface...\n");
        return 1;
    }

    delete_password(pass_name);
    
    return 0;
}

int cmd_interface(int *exit) {
    printf(LOCKSMITH_PROMPT1);
    char command[MAX_STRING_LEN];
    safe_scanf(MAX_STRING_LEN, "%s", command);

    switch (get_cmd_value(command)) {
        case CMD_NEW:
            cmd_create_password();
            break;
        case CMD_GET:
            cmd_get_password();
            break;
        case CMD_DEL:
            cmd_delete_password();
            break;
        case CMD_HELP:
            printf(LIST_OF_COMMANDS);
            break;
        case CMD_EXIT:
            printf("Exiting...\n");
            *exit = 1;
            break;
        case INVALID:
            cli_error("Invalid command!");
            break;
        default:
            die("Something went wrong while handling user input.");
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

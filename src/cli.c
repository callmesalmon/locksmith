#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/time.h>
#include <stdarg.h>

#include "cli.h"
#include "commons.h"
#include "crypto.h"
#include "password.h"
#include "colors.h"

#define LIST_OF_COMMANDS          \
"new  - create new password\n"    \
"get  - get password\n"           \
"del  - delete password\n"        \
"list - list passwords\n"         \
"exit - exit command interface\n" \
"help - show this text\n"         \

static unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];

int cli_init() {
    get_key(key);

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

int cli_info(char *message) {
    printf_color(GREEN, "INFO: %s", message);
    return 0;
}

typedef enum {
    CMD_NEW,
    CMD_GET,
    CMD_DEL,
    CMD_LIST,
    CMD_EXIT,
    CMD_HELP,

    EMPTY,
    INVALID,
} CommandList;

CommandList get_cmd_value(char str[MAX_STRING_LEN]) {
    if (!strcmp(str, "new"))  return CMD_NEW;
    if (!strcmp(str, "get"))  return CMD_GET;
    if (!strcmp(str, "del"))  return CMD_DEL;
    if (!strcmp(str, "list")) return CMD_LIST;
    if (!strcmp(str, "exit")) return CMD_EXIT;
    if (!strcmp(str, "help")) return CMD_HELP;
    if (!strcmp(str, ""))     return EMPTY;
    return INVALID;
}

char *format_password_filename(char *site, char *user) {
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

    printf("sitename: ");
    get_string(site_name);

    printf("username: ");
    get_string(user_name);

    printf("password: ");
    get_string(password);

    // This is not a good implementation. In reality we should just remove
    // the automatic prepending of LOCKSMITH_DIR* to all filenames.
    char *full_filename = format_password_filename(site_name, user_name);
    char full_filename_with_dir[MAX_STRING_LEN];
    snprintf(full_filename_with_dir, MAX_STRING_LEN, "%s%s.txt", LOCKSMITH_PASSW_DIR, full_filename);
    
    if (fexists(full_filename_with_dir)) {
        cli_warning("Password file already exists!\n"
                    "Are you sure you want to overwrite this password? [y/N] ");

        char overwrite_pass[MAX_STRING_LEN];
        get_string_color(YELLOW, overwrite_pass);
        
        if (strcmp(overwrite_pass, "y") != 0) return 0;
    }

    create_password(full_filename, password, key);

    return 0;
}

int cmd_get_password() {
    char pass_name[MAX_STRING_LEN];

    printf("enter password name: ");
    get_string(pass_name);

    if (!passname_handler(pass_name)) {
        cli_error("Password file not accessible!\n"
                  "Returning to command interface...\n");
        return 1;
    }

    printf("password:");
    printf(" %s\n", get_password(pass_name, key));

    return 0;
}

int cmd_delete_password() {
    char pass_name[MAX_STRING_LEN];

    printf("enter password name: ");
    get_string(pass_name);

    if (!passname_handler(pass_name)) {
        cli_error("Password file not accessible!\n"
                  "Returning to command interface...\n");
        return 1;
    }

    delete_password(pass_name);
    
    return 0;
}

int cmd_list_passwords() {
    printf("list of passwords:\n");
    list_passwords();

    return 0;
}

int cmd_interface(int *exit) {
    printf(LOCKSMITH_PROMPT);

    char command[MAX_STRING_LEN];
    command[0] = '\0'; // Make sure command is empty.

    get_string(command);

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
        case CMD_LIST:
            cmd_list_passwords();
            break;
        case CMD_HELP:
            printf(LIST_OF_COMMANDS);
            break;
        case CMD_EXIT:
            cli_info("Exiting...\n");
            *exit = 1;
            break;
        case EMPTY:
            break;
        case INVALID:
            cli_error("Invalid command!");
            break;
        default:
            die("Something went wrong while handling user input.\nYou should be worried.\n");
    }

    return 0;
}

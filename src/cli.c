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

#define LIST_OF_COMMANDS                   \
"new - create new password\n"              \
"get - get password\n"                     \
"del - delete password\n"                  \
"enable_verbose - enable verbose mode\n"   \
"disable_verbose - disable verbose mode\n" \
"exit - exit command interface\n"          \
"help - show this text\n"

static unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];
static int           is_verbose = 0;

typedef enum {
    CMD_NEW,
    CMD_GET,
    CMD_DEL,
    CMD_ENBL_VERBOSE,
    CMD_DISBL_VERBOSE,
    CMD_EXIT,
    CMD_HELP,

    EMPTY,
    INVALID,
} CommandList;

CommandList get_cmd_value(char str[MAX_STRING_LEN]) {
    if (!strcmp(str, "new"))             return CMD_NEW;
    if (!strcmp(str, "get"))             return CMD_GET;
    if (!strcmp(str, "del"))             return CMD_DEL;
    if (!strcmp(str, "enable_verbose"))  return CMD_ENBL_VERBOSE;
    if (!strcmp(str, "disable_verbose")) return CMD_DISBL_VERBOSE;
    if (!strcmp(str, "exit"))            return CMD_EXIT;
    if (!strcmp(str, "help"))            return CMD_HELP;
    if (!strcmp(str, ""))                return EMPTY;
    return INVALID;
}

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

int verbose_log(char *format, ...) {
    va_list args;

    va_start(args, format);
    va_end(args);

    if (is_verbose) {
        vprintf(format, args);
    }

    return 0;
}

char *format_password_filename(char *site, char *user) {
    static char full_filename[MAX_STRING_LEN];
    snprintf(full_filename, sizeof(full_filename), "%s:%s", site, user);

    verbose_log("full_filename: %s\n", full_filename);

    return full_filename;
}

// We need to check if passname + .txt exists
int passname_handler(char *passname) {
    static char full_filename[MAX_STRING_LEN];
    snprintf(full_filename, MAX_STRING_LEN, "%s%s.txt", LOCKSMITH_PASSW_DIR, passname);

    verbose_log("full_filename: %s\n", full_filename);

    if (fexists(full_filename)) {
        return 1;
    }
    return 0;
}

int cmd_create_password() {
    char site_name[MAX_STRING_LEN], user_name[MAX_STRING_LEN], password[MAX_STRING_LEN];

    printf("\nSitename: ");
    printf(LOCKSMITH_PROMPT2);
    safe_scanf(MAX_STRING_LEN, "%s", site_name);

    printf("\nUsername: ");
    printf(LOCKSMITH_PROMPT2);
    safe_scanf(MAX_STRING_LEN, "%s", user_name);

    printf("\nPassword: ");
    printf(LOCKSMITH_PROMPT2);
    safe_scanf(MAX_STRING_LEN, "%s", password);

    // This is not a good implementation. In reality we should just remove
    // the automatic prepending of LOCKSMITH_DIR* to all filenames.
    char *full_filename = format_password_filename(site_name, user_name);
    char full_filename_with_dir[MAX_STRING_LEN];
    snprintf(full_filename_with_dir, MAX_STRING_LEN, "%s%s.txt", LOCKSMITH_PASSW_DIR, full_filename);

    verbose_log("full_filename_with_dir: %s\n", full_filename_with_dir);
    verbose_log("site: %s | user: %s | pass: %s\n", site_name, user_name, password);
    
    if (fexists(full_filename_with_dir)) {
        cli_warning("Password file already exists!\n"
                    "Are you sure you want to overwrite this password? [y/N] ");

        char overwrite_pass[MAX_STRING_LEN];
        safe_scanf(MAX_STRING_LEN, "%s", overwrite_pass);

        verbose_log("overwrite_pass: %s\n", overwrite_pass);
        
        if (strcmp(overwrite_pass, "y") != 0) return 0;
    }

    create_password(full_filename, password, key);

    return 0;
}

int cmd_get_password() {
    char pass_name[MAX_STRING_LEN];

    printf("\nWhat password do you want to get?\n");
    list_passwords();
    printf(LOCKSMITH_PROMPT2);
    safe_scanf(MAX_STRING_LEN, "%s", pass_name);

    verbose_log("pass_name: %s\n", pass_name);

    if (!passname_handler(pass_name)) {
        cli_error("Password file not accessible!\n"
                  "Returning to command interface...\n");
        return 1;
    }

    printf_color(UNDERLINE_WHITE, "Password:");
    printf(" %s\n", get_password(pass_name, key));

    verbose_log("get_password: '%s'\n", get_password(pass_name, key));

    return 0;
}

int cmd_delete_password() {
    char pass_name[MAX_STRING_LEN];

    printf("\nWhat password do you want to delete?\n");
    list_passwords();
    printf(LOCKSMITH_PROMPT2);
    printf(">");
    safe_scanf(MAX_STRING_LEN, "%s", pass_name);

    verbose_log("pass_name: %s\n", pass_name);

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
    command[0] = '\0'; // Make sure command is empty.
    
    safe_scanf(MAX_STRING_LEN, "%s", command);

    verbose_log("Read '%s' into 'command'.\n", command);
    verbose_log("Command value of 'command' is %d.\n", get_cmd_value(command));

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
        case CMD_ENBL_VERBOSE:
            is_verbose = 1;
            break;
        case CMD_DISBL_VERBOSE:
            is_verbose = 0;
            break;
        case CMD_HELP:
            printf(LIST_OF_COMMANDS);
            break;
        case CMD_EXIT:
            printf("Exiting...\n");
            *exit = 1;
            break;
        case EMPTY:
            break;
        case INVALID:
            cli_error("Invalid command!");
            break;
        default:
            die("Something went wrong while handling user input.");
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>

#include "cli.h"
#include "cli_msg.h"
#include "commons.h"
#include "crypto.h"
#include "password.h"

#define LIST_OF_COMMANDS                \
"new        create new password\n"      \
"get        get password\n"             \
"recover    recover password\n"         \
"del        delete password\n"          \
"list       list passwords\n"           \
"exit       exit command interface\n"   \
"help       show this text\n"           \

static unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];

int cli_init() {
    get_key(key);

    return 0;
}

typedef enum {
    CMD_NEW,
    CMD_GET,
    CMD_RECOVER,
    CMD_DEL,
    CMD_LIST,
    CMD_EXIT,
    CMD_HELP,

    EMPTY,
    INVALID,
} CommandList;

CommandList get_cmd_value(char str[MAX_STRING_LEN]) {
    if (!strcmp(str, "new"))        return CMD_NEW;
    if (!strcmp(str, "get"))        return CMD_GET;
    if (!strcmp(str, "recover"))    return CMD_RECOVER;
    if (!strcmp(str, "del"))        return CMD_DEL;
    if (!strcmp(str, "list"))       return CMD_LIST;
    if (!strcmp(str, "exit"))       return CMD_EXIT;
    if (!strcmp(str, "help"))       return CMD_HELP;
    if (!strcmp(str, ""))           return EMPTY;
    return INVALID;
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
    char *full_filename          = format_password_filename(site_name, user_name);
    char *full_filename_with_dir = password_file(full_filename);
    
    if (fexists(full_filename_with_dir)) {
        int answer = cli_warn_yes_no(
            "Password file already exists!\n"
            "Are you sure you want to overwrite this password? [y/N] "
        );
        if (!answer) return 0;
    }

    create_password(full_filename, password, key);
    backup_password(full_filename, password, key);

    return 0;
}

int cmd_get_password() {
    char pass_name[MAX_STRING_LEN];

    printf("enter password name: ");
    get_string(pass_name);

    if (!password_exists(pass_name)) {
        return cli_error("Password file not accessible!\n"
                  "Returning to command interface...\n");
    }

    printf("password:");
    printf(" %s\n", get_password(pass_name, key));

    return 0;
}

int cmd_recover_password() {
    char bak_name[MAX_STRING_LEN];

    printf("enter password name: ");
    get_string(bak_name);

    if (password_exists(password_file(bak_name))) {
        int answer = cli_warn_yes_no(
            "A password file matching your input query already exists\n"
            "Are you sure you want to overwrite this password with the backup? [y/N] "
        );
        if (!answer) return 0;
    }

    recover_password(bak_name);

    return 0;
}

int cmd_delete_password() {
    char pass_name[MAX_STRING_LEN];

    printf("enter password name: ");
    get_string(pass_name);

    if (!password_exists(pass_name)) {
        return cli_error("Password file not accessible!\n"
                  "Returning to command interface...\n");
    }

    int answer = cli_warn_yes_no(
        "This will DELETE YOUR STORED PASSWORD!\n"
        "(Until you've closed the program, you can still recover the password with 'recover')\n"
        "Are you sure you want to proceed? [y/N] "
        
    );
    if (!answer) return 0;

    delete_password(pass_name);

    return 0;
}

int cmd_list_passwords() {
    printf("list of passwords:\n");
    list_passwords();

    return 0;
}

int cmd_interface() {
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
        case CMD_RECOVER:
            cmd_recover_password();
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
            exit(0);
        case EMPTY:
            break;
        case INVALID:
            cli_error("Invalid command!\n");
            break;
        default:
            die("Something went wrong while handling user input.\nYou should be worried.\n");
    }

    return 0;
}

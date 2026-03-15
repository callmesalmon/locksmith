#include <stdio.h>
#include <stdlib.h>

#include "cli.h"
#include "cli_msg.h"
#include "auth.h"
#include "commons.h"
#include "crypto.h"
#include "password.h"

/**** Initialization ****/

static unsigned char key[KEY_LEN];

int cli_init() {
    get_key(key);

    return 0;
}

/**** Command framework ****/

#define LIST_OF_COMMANDS                    \
"new            create new password\n"      \
"get            get password\n"             \
"recover        recover password\n"         \
"del            delete password\n"          \
"list           list passwords\n"           \
"listbak        list backups\n"             \
"masterpasswd   change master password\n"   \
"exit           exit command interface\n"   \
"help           show this text\n"           \

typedef enum {
    CMD_NEW,
    CMD_GET,
    CMD_RECOVER,
    CMD_DEL,
    CMD_LIST,
    CMD_LISTBAK,
    CMD_CHMPASS,
    CMD_EXIT,
    CMD_HELP,

    EMPTY,
    INVALID,
} CommandList;

CommandList get_cmd_value(char str[MAX_STRING_LEN]) {
    if (!strcmp(str, "new"))            return CMD_NEW;
    if (!strcmp(str, "get"))            return CMD_GET;
    if (!strcmp(str, "recover"))        return CMD_RECOVER;
    if (!strcmp(str, "del"))            return CMD_DEL;
    if (!strcmp(str, "list"))           return CMD_LIST;
    if (!strcmp(str, "listbak"))        return CMD_LISTBAK;
    if (!strcmp(str, "masterpasswd"))   return CMD_CHMPASS;
    if (!strcmp(str, "exit"))           return CMD_EXIT;
    if (!strcmp(str, "help"))           return CMD_HELP;
    if (!strcmp(str, ""))               return EMPTY;
    return INVALID;
}

/**** Shell commands ****/

int cmd_create_password() {
    Password pass;
    char retype[MAX_STRING_LEN];

    printf("Title: ");
    get_string(pass.title);

    printf("Username: ");
    get_string(pass.username);

    printf("URL: ");
    get_string(pass.url);

    printf("Password: ");
    get_string_secret(pass.password);

    printf("Retype: ");
    get_string_secret(retype);

    if (strcmp(pass.password, retype) != 0) {
        return cli_error("Passwords don't match!\n");
    }

    // This is not a good implementation. In reality we should just remove
    // the automatic prepending of LOCKSMITH_DIR* to all filenames.
    char *title_with_dir = password_file(pass.title);

    if (fexists(title_with_dir)) {
        int answer = cli_warn_yes_no(
            "Password file already exists!\n"
            "Are you sure you want to overwrite this password? [y/N] "
        );
        if (!answer) return 0;
    }

    create_password(pass, key);
    backup_password(pass);

    return 0;
}

int cmd_get_password() {
    char pass_name[MAX_STRING_LEN];

    printf("Title: ");
    get_string(pass_name);

    if (!password_exists(pass_name)) {
        return cli_error("Password file not accessible!\n"
                  "Returning to command interface...\n");
    }

    Password pass = get_password(pass_name, key);

    // This... should work...
    if (pass.title == (char *)NULL) {
        return cli_error("Couldn't get password!\n");
    }

    printf("Username: %s\n", pass.username);
    printf("URL: %s", pass.url); // praying that this always works
    printf("Password: %s\n", pass.password);

    return 0;
}

int cmd_recover_password() {
    char bak_name[MAX_STRING_LEN];

    printf("Title: ");
    get_string(bak_name);

    if (password_exists(password_file(bak_name))) {
        int answer = cli_warn_yes_no(
            "A password file matching your input query already exists!\n"
            "Are you sure you want to overwrite this password with the backup? [y/N] "
        );
        if (!answer) return 0;
    }

    recover_password(bak_name);

    return 0;
}

int cmd_delete_password() {
    char pass_name[MAX_STRING_LEN];

    printf("Title: ");
    get_string(pass_name);

    if (!password_exists(pass_name)) {
        return cli_error("Password file not accessible!\n"
                  "Returning to command interface...\n");
    }

    int answer = cli_warn_yes_no(
        "This will DELETE YOUR STORED PASSWORD!\n"
        "Are you sure you want to proceed? [y/N] "
    );
    if (!answer) return 0;

    int result = delete_password(pass_name);
    if (result == -1) {
        return cli_error("Couldn't delete password!\n");
    }

    return 0;
}

int cmd_list_passwords() {
    printf("List of passwords:\n");
    list_passwords();

    return 0;
}

int cmd_list_backups() {
    printf("List of backups:\n");
    list_backups();

    return 0;
}

int cmd_change_master_password() {
    MasterPassword mpa;
    mpa.fptr = fopen(LOCKSMITH_MASTER_PASSW_FILE, "rb");

    fread(mpa.hash, 1, MAX_HASH_LEN, mpa.fptr);

    printf("Old master password: ");
    get_string_secret(mpa.password);

    if (!master_password_correct(mpa)) {
        cli_error("Wrong master password!\n");
        return 0;
    }

    char new_master_password[MAX_STRING_LEN];
    char new_master_password_retype[MAX_STRING_LEN];

    printf("New master password: ");
    get_string_secret(new_master_password);

    printf("Retype master password: ");
    get_string_secret(new_master_password_retype);

    int result = create_master_password(new_master_password, new_master_password_retype);
    if (result == -1) {
        cli_error("Passwords don't match!\n");
        return -1;
    }

    return 0;
}

/**** Main shell entrypoint ****/

int cmd_interface() {
    printf(LOCKSMITH_PROMPT);

    char command[MAX_STRING_LEN];
    command[0] = '\0'; // Make sure command is empty.

    get_string(command);

    // There is probably a prettier way to go about this
    // but switch statements are BLAZINGLY FAST so it's ok
    // I think
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
        case CMD_LISTBAK:
            cmd_list_backups();
            break;
        case CMD_CHMPASS:
            cmd_change_master_password();
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
            cli_error_die(-1, "Unhandled case!\n");
    }

    return 0;
}

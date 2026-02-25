#ifndef CLI_H
#define CLI_H

#include <sodium.h>

#include "crypto.h"

#define LOCKSMITH_PROMPT1 PURPLE "\nlocksmith> " DEFAULT_COLOR
#define LOCKSMITH_PROMPT2 "\n> "

typedef enum {
    CMD_NEW,
    CMD_GET,
    CMD_DEL,
    CMD_EXIT,
    CMD_HELP,

    EMPTY,
    INVALID,
} CommandList;

CommandList get_cmd_value(char str[MAX_STRING_LEN]);

int cli_init();

int cli_error(char *message);
int cli_warning(char *message);

int cmd_create_password();
int cmd_get_password();
int cmd_delete_password();

int cmd_interface(int *exit);

#endif

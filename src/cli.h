#ifndef CLI_H
#define CLI_H

#include <sodium.h>

#include "crypto.h"

#define LOCKSMITH_PROMPT PURPLE "\nlocksmith> " DEFAULT_COLOR

int cli_init();

int cli_error(char *message);
int cli_warning(char *message);

int cmd_create_password();
int cmd_get_password();
int cmd_delete_password();
int cmd_list_passwords();

int cmd_interface(int *exit);

#endif

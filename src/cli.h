#ifndef CLI_H
#define CLI_H

#include "colors.h"

#define LOCKSMITH_PROMPT PURPLE "locksmith> " DEFAULT_COLOR

int cli_init();

int cmd_create_password();
int cmd_get_password();
int cmd_recover_password();
int cmd_delete_password();
int cmd_list_passwords();

int cmd_interface();

#endif

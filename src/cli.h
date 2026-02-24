#ifndef CLI_H
#define CLI_H

#include <sodium.h>

int cli_init();
int cmd_create_password();
int cmd_get_password();
int cmd_delete_password();
int cmd_interface(int *exit);

#endif

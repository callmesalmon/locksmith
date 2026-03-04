#ifndef CLI_MSG_H
#define CLI_MSG_H

int cli_error(char *message);
int cli_warn(char *message);
int cli_info(char *message);
int cli_warn_yes_no(char *message);

#endif

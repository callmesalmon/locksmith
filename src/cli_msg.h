#ifndef CLI_MSG_H
#define CLI_MSG_H

int cli_error(const char *format, ...);
void cli_error_die(int exit_code, const char *format, ...);

int cli_warn(const char *format, ...);
int cli_warn_yes_no(const char *format, ...);

int cli_info(const char *format, ...);

#endif

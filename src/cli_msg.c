#include <string.h>

#include "cli_msg.h"
#include "commons.h"
#include "crypto.h"

int cli_error(char *message) {
    printf_color(RED, "ERROR: %s", message);
    return 0;
}

int cli_warn(char *message) {
    printf_color(YELLOW, "WARNING: %s", message);
    return 0;
}

int cli_info(char *message) {
    printf_color(GREEN, "INFO: %s", message);
    return 0;
}

int cli_warn_yes_no(char *message) {
    cli_warn(message);

    char answer[MAX_STRING_LEN];
    get_string_color(YELLOW, answer);

    // assumes N
    if (strcmp(answer, "y") != 0) return 0;
    return 1;
}

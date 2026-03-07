#include <string.h>
#include <stdarg.h>

#include "cli_msg.h"
#include "commons.h"
#include "crypto.h"
#include "colors.h"

int cli_error(const char *format, ...) {
    va_list args;

    va_start(args, format);
    va_end(args);

    printf_color(RED, "ERROR: ");
    vprintf_color(RED, format, args);

    return 0;
}

void cli_error_die(int exit_code, const char *format, ...) {
    va_list args;

    va_start(args, format);
    va_end(args);

    cli_error(format, args);
    exit(exit_code);
}

int cli_warn(const char *format, ...) {
    va_list args;

    va_start(args, format);
    va_end(args);

    printf_color(YELLOW, "WARNING: ");
    vprintf_color(YELLOW, format, args);

    return 0;
}

int cli_info(const char *format, ...) {
    va_list args;

    va_start(args, format);
    va_end(args);

    printf_color(GREEN, "INFO: ");
    printf_color(GREEN, format, args);

    return 0;
}

int cli_warn_yes_no(const char *format, ...) {
    va_list args;

    va_start(args, format);
    va_end(args);

    cli_warn(format, args);

    char answer[MAX_STRING_LEN];
    get_string_color(YELLOW, answer);

    // assumes N
    if (strcmp(answer, "y") != 0) return 0;
    return 1;
}

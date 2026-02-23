#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#include "commons.h"

char *get_home_dir() {
    return getpwuid(getuid())->pw_dir;
}

int directory_exists(char *dirname) {
    struct stat st;
    if (stat(dirname, &st) == 0) {
        return 1;
    }
    return 0;
}

int mkdirifnotexist(char *dirname) {
    if (directory_exists(dirname) == 0) {
        mkdir(dirname, 0755);
    }

    return 0;
}

int safe_scanf(int limit, const char *format, ...) {
    va_list args;

    va_start(args, format);
    va_end(args);

    char buff[limit];

    if (!fgets(buff, limit, stdin)) {
        errno = 2;
        return -1;
    }
    fflush(stdin); // fgets() doesn't flush excess chars

    buff[strcspn(buff, "\n")] = 0; // Remove newline
    vsscanf(buff, format, args);

    return 0;
}

int safe_srand() {
    int pid = getpid();
    struct timeval t;
    gettimeofday(&t, NULL);
    srand(t.tv_usec ^ t.tv_sec ^ pid);

    return 0;
}

int strip_ext(char *fname) {
    char *end = fname + strlen(fname);

    while (end > fname && *end != '.' && *end != '\\' && *end != '/') {
        --end;
    }
    if ((end > fname && *end == '.') &&
        (*(end - 1) != '\\' && *(end - 1) != '/')) {
        *end = '\0';
    }

    return 0;
}

int printf_color(char *color, const char *format, ...) {
    va_list args;

    va_start(args, format);
    va_end(args);

    
    printf("%s", color);
    vprintf(format, args);
    printf("%s", DEFAULT_COLOR);

    return 0;
}

int fexists(char *fname) {
  struct stat   buffer;   
  return (stat (fname, &buffer) == 0);
}

void die(char *format, ...) {
    va_list args;

    va_start(args, format);
    va_end(args);

    if(errno) {
        perror(format);
    } else {
        vprintf(format, args);
    }

    exit(1); // We exit, not return.
}

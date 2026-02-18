#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <stdarg.h>

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

int rand_range(int min, int max) {
    unsigned int seed = time(0);
    return rand_r(&seed) % (max - min + 1) + min;
}

int mkdirifnotexist(char *dirname) {
    if (directory_exists(dirname) == 0) {
        mkdir(dirname, 0755);
    }

    return 0;
}

void die(char *format, ...) {
    va_list args;

    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    if(errno) {
        perror(format);
    } else {
        vprintf(format, args);
    }
    exit(1); // We exit, not return.
}

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <stdarg.h>
#include <string.h>
#include <termios.h>
#include <stdlib.h>

#include "commons.h"
#include "colors.h"
#include "crypto.h"

/**** Directory-related ****/

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

int mkdir_if_not_exist(char *dirname) {
    if (directory_exists(dirname) == 0) {
        mkdir(dirname, 0755);
        return 0;
    }

    return -1;
}

int delete_all_in_dir(char *dirname) {
    DIR *dir = opendir(dirname);
    struct dirent *de;

    while ((de = readdir(dir)) != NULL) {
        char *fname = de->d_name;

        char full_fname[(MAX_STRING_LEN * 2) + 1];
        snprintf(full_fname, sizeof(full_fname), "%s/%s", dirname, fname);
        
        int special = (!strcmp(fname, ".") || !strcmp(fname, ".."));

        if (!special) {
            int result = remove(full_fname);
            if (result != 0)
                return -1;
        }
    }

    return 0;
}

int list_dir(char *dirname) {
    struct dirent *de;

    DIR *dir = opendir(dirname);

    if (dir == NULL) {
        return -1;
    }

    while ((de = readdir(dir)) != NULL) {
        char *fname = de->d_name;

        int special = (!strcmp(fname, ".") || !strcmp(fname, ".."));

        // fname != "." is for the "." (cwd) and ".." ""files""
        if (fname != NULL && !special) printf("%s\n", fname);
    }

    closedir(dir);

    return 0;
}

/**** File-related ****/

int file_write(char *path, char content[]) {
    FILE *fptr = fopen(path, "w");
    if (fptr == NULL)
        return -1;

    fprintf(fptr, "%s", content);
    fclose(fptr);

    return 0;
}

int file_copy(char *dest, char *src) {
    FILE *fptr = fopen(src, "rb");
    if (fptr == NULL)
        return -1;

    char buf[MAX_STRING_LEN * 2];
    fread(buf, 1, MAX_STRING_LEN, fptr);
    
    fclose(fptr);

    fptr = fopen(dest, "w");
    if (fptr == NULL)
        return -1;

    fprintf(fptr, "%s", buf);

    fclose(fptr);

    return 0;
}

int fexists(char *fname) {
  struct stat   buffer;
  return (stat (fname, &buffer) == 0);
}

/**** String-related ****/

int safe_scanf(int limit, const char *format, ...) {
    va_list args;

    va_start(args, format);
    va_end(args);

    char buff[limit];

    if (!fgets(buff, limit, stdin)) {
        return -1;
    }
    fflush(stdin); // fgets() doesn't flush excess chars

    buff[strcspn(buff, "\n")] = 0; // Remove newline
    vsscanf(buff, format, args);

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

int vprintf_color(char *color, const char *format, va_list args) {
    printf("%s", color);
    vprintf(format, args);
    printf("%s", DEFAULT_COLOR);

    return 0;
}

// safe_scanf but for the very common use case
// of wanting to just get a string.
int get_string(char str[MAX_STRING_LEN]) {
    safe_scanf(MAX_STRING_LEN, "%[^\n]", str);

    return 0;
}

int get_string_secret(char str[MAX_STRING_LEN]) {
    struct termios t_new, t_old;
    tcgetattr(STDIN_FILENO, &t_new);
    tcgetattr(STDIN_FILENO, &t_old);

    t_new.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &t_new);

    get_string(str);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &t_old);

    printf("\n"); // to simulate the user entering input... we need this to look pretty

    return 0;
}

int get_string_color(char *color, char str[MAX_STRING_LEN]) {
    printf("%s", color);
    get_string(str);
    printf(DEFAULT_COLOR);

    return 0;
}

/**** Misc. ****/

void die(char *format, ...) {
    va_list args;

    va_start(args, format);
    va_end(args);

    vprintf(format, args);

    exit(1); // We exit, not return.
}

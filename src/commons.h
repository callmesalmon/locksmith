#ifndef COMMONS_H
#define COMMONS_H

#include <dirent.h>
#include <stdarg.h>

#include "crypto.h"

char *get_home_dir();
int directory_exists(char *dirname);
int mkdir_if_not_exist(char *dirname);
int delete_all_in_dir(char *dirname);
int list_dir(char *dirname);

int file_write(char *path, char content[]);
int file_copy(char *dest, char *src);
int fexists(char *fname);

int safe_scanf(int limit, const char *format, ...);
int strip_ext(char *fname);
int printf_color(char *color, const char *format, ...);
int vprintf_color(char *color, const char *format, va_list args);
int get_string(char str[MAX_STRING_LEN]);
int get_string_secret(char str[MAX_STRING_LEN]);
int get_string_color(char *color, char str[MAX_STRING_LEN]);

void die(char *format, ...);

#endif

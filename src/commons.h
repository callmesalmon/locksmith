#ifndef COMMONS_H
#define COMMONS_H

#include <stdio.h>

#include "colors.h"
#include "crypto.h"

char *get_home_dir();
int directory_exists(char *dirname);
int mkdirifnotexist(char *dirname);

int safe_scanf(int limit, const char *format, ...);

int strip_ext(char *fname);
int printf_color(char *color, const char *format, ...);
int vprintf_color(char *color, const char *format, va_list args);
int get_string(char str[MAX_STRING_LEN]);
int get_string_color(char *color, char str[MAX_STRING_LEN]);

int fexists(char *fname);

void die(char *format, ...);

#endif

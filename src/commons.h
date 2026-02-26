#ifndef COMMONS_H
#define COMMONS_H

#include <stdio.h>

#include "colors.h"
#include "crypto.h"

char *get_home_dir();
int directory_exists(char *dirname);
int mkdirifnotexist(char *dirname);
int safe_scanf(int limit, const char *format, ...);
int safe_srand();
int get_string(char str[MAX_STRING_LEN]);
int strip_ext(char *fname);
int printf_color(char *color, const char *format, ...);
int fexists(char *fname);
void die(char *format, ...);

#endif

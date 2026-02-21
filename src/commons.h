#ifndef COMMONS_H
#define COMMONS_H

char *get_home_dir();
int directory_exists(char *dirname);
int rand_range(int min, int max);
int mkdirifnotexist(char *dirname);
int safe_scanf(int limit, const char *format, ...);
int safe_srand();
int strip_ext(char *fname);
void die(char *format, ...);

#endif

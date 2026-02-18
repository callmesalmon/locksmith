#ifndef COMMONS_H
#define COMMONS_H

#define VERBOSE 0 // Change to 1 to turn on debug/verbose mode.
#define IFVERBOSE(...) if (VERBOSE) printf(__VA_ARGS__);

char *get_home_dir();
int directory_exists(char *dirname);
int rand_range(int min, int max);
int mkdirifnotexist(char *dirname);
void die(char *format, ...);

#endif

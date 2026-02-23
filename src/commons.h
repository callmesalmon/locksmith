#ifndef COMMONS_H
#define COMMONS_H

#include <stdio.h>

#define BLACK               "\033[0;30m"
#define RED                 "\033[0;31m"
#define GREEN               "\033[0;32m"
#define YELLOW              "\033[0;33m" 
#define BLUE                "\033[0;34m"
#define PURPLE              "\033[0;35m"
#define CYAN                "\033[0;36m"
#define WHITE               "\033[0;37m"

#define BOLD_BLACK          "\033[1;30m"
#define BOLD_RED            "\033[1;31m"
#define BOLD_GREEN          "\033[1;32m"
#define BOLD_YELLOW         "\033[1;33m"
#define BOLD_BLUE           "\033[1;34m"
#define BOLD_PURPLE         "\033[1;35m"
#define BOLD_CYAN           "\033[1;36m"
#define BOLD_WHITE          "\033[1;37m"

#define UNDERLINE_BLACK     "\033[4;30m"
#define UNDERLINE_RED       "\033[4;31m"
#define UNDERLINE_GREEN     "\033[4;32m"
#define UNDERLINE_YELLOW    "\033[4;33m"
#define UNDERLINE_BLUE      "\033[4;34m"
#define UNDERLINE_PURPLE    "\033[4;35m"
#define UNDERLINE_CYAN      "\033[4;36m"
#define UNDERLINE_WHITE     "\033[4;37m"

#define BACKGROUND_BLACK    "\033[40m"
#define BACKGROUND_RED      "\033[41m"
#define BACKGROUND_GREEN    "\033[42m"
#define BACKGROUND_YELLOW   "\033[43m"
#define BACKGROUND_BLUE     "\033[44m"
#define BACKGROUND_PURPLE   "\033[45m"
#define BACKGROUND_CYAN     "\033[46m"
#define BACKGROUND_WHITE    "\033[47m"

#define HIGH_INTENSTY_BACKGROUND_BLACK      "\033[0;100m"
#define HIGH_INTENSTY_BACKGROUND_RED        "\033[0;101m"
#define HIGH_INTENSTY_BACKGROUND_GREEN      "\033[0;102m"
#define HIGH_INTENSTY_BACKGROUND_YELLOW     "\033[0;103m"
#define HIGH_INTENSTY_BACKGROUND_BLUE       "\033[0;104m"
#define HIGH_INTENSTY_BACKGROUND_PURPLE     "\033[0;105m"
#define HIGH_INTENSTY_BACKGROUND_CYAN       "\033[0;106m"
#define HIGH_INTENSTY_BACKGROUND_WHITE      "\033[0;107m"

#define HIGH_INTENSTY_TEXT_BLACK            "\033[0;90m"
#define HIGH_INTENSTY_TEXT_RED              "\033[0;91m"
#define HIGH_INTENSTY_TEXT_GREEN            "\033[0;92m"
#define HIGH_INTENSTY_TEXT_YELLOW           "\033[0;93m"
#define HIGH_INTENSTY_TEXT_BLUE             "\033[0;94m"
#define HIGH_INTENSTY_TEXT_PURPLE           "\033[0;95m"
#define HIGH_INTENSTY_TEXT_CYAN             "\033[0;96m"
#define HIGH_INTENSTY_TEXT_WHITE            "\033[0;97m"

#define BOLD_HIGH_INTENSITY_TEXT_BLACK      "\033[1;90m"
#define BOLD_HIGH_INTENSITY_TEXT_RED        "\033[1;91m"
#define BOLD_HIGH_INTENSITY_TEXT_GREEN      "\033[1;92m"
#define BOLD_HIGH_INTENSITY_TEXT_YELLOW     "\033[1;93m"
#define BOLD_HIGH_INTENSITY_TEXT_BLUE       "\033[1;94m"
#define BOLD_HIGH_INTENSITY_TEXT_PURPLE     "\033[1;95m"
#define BOLD_HIGH_INTENSITY_TEXT_CYAN       "\033[1;96m"
#define BOLD_HIGH_INTENSITY_TEXT_WHITE      "\033[1;97m"

#define DEFAULT_COLOR                       "\033[0m"

char *get_home_dir();
int directory_exists(char *dirname);
int mkdirifnotexist(char *dirname);
int safe_scanf(int limit, const char *format, ...);
int safe_srand();
int strip_ext(char *fname);
int printf_color(char *color, const char *format, ...);
int fexists(char *fname);
void die(char *format, ...);

#endif

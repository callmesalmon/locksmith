//    Copyright (C) 2026, Salmon
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/time.h>
#include <dirent.h>
#include <stdarg.h>

#include "commons.h"
#include "password.h"
#include "cli.h"

#define locksmith_title \
"88                                88                                     88         88         \n" \
"88                                88                                     \"\"   ,d    88         \n" \
"88                                88                                          88    88         \n" \
"88          ,adPPYba,   ,adPPYba, 88   ,d8  ,adPPYba, 88,dPYba,,adPYba,  88 MM88MMM 88,dPPYba, \n" \
"88         a8\"     \"8a a8\"     \"\" 88 ,a8\"   I8[    \"\" 88P'   \"88\"    \"8a 88   88    88P'    \"8a\n" \
"88         8b       d8 8b         8888[      `\"Y8ba,  88      88      88 88   88    88       88\n" \
"88         \"8a,   ,a8\" \"8a,   ,aa 88`\"Yba,  aa    ]8I 88      88      88 88   88,   88       88\n" \
"88888888888 `\"YbbdP\"'   \"Ybbd8\"' 88   `Y8a `\"YbbdP\"' 88      88      88 88   \"Y888 88       88\n\n" \



int main(int argc, char **argv) {
    safe_srand();

    mkdirifnotexist(locksmith_dir);
    mkdirifnotexist(locksmith_passw_dir);

    printf(locksmith_title);

    verify_master_password_interface();

    int exit_cmd = 0;
    while (!exit_cmd) {
        cmd_interface(&exit_cmd);
    }

    return 0;
}

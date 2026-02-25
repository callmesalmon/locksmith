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
#include <sodium.h>

#include "auth.h"
#include "colors.h"
#include "commons.h"
#include "password.h"
#include "cli.h"
#include "crypto.h"

#define LOCKSMITH_TITLE "\n"                                  \
"    __               __                  _ __  __      \n"   \
"   / /   ____  _____/ /___________ ___  (_) /_/ /_     \n"   \
"  / /   / __ \\/ ___/ //_/ ___/ __ `__ \\/ / __/ __ \\ \n"   \
" / /___/ /_/ / /__/ ,< (__  ) / / / / / / /_/ / / /    \n"   \
"/_____/\\____/\\___/_/|_/____/_/ /_/ /_/_/\\__/_/ /_/  \n\n" 
#define LOCKSMITH_VERSION "1.1"

int main(int argc, char **argv) {
    safe_srand();

    mkdirifnotexist(LOCKSMITH_DIR);
    mkdirifnotexist(LOCKSMITH_PASSW_DIR);

    create_key(LOCKSMITH_KEY_FILE);

    cli_init();
    auth_master_password();

    printf_color(CYAN, LOCKSMITH_TITLE);

    printf("Locksmith version %s\n", LOCKSMITH_VERSION);
    printf("License: GPLv2-or-later <https://spdx.org/licenses/GPL-2.0-or-later.html> (original code),\n"
           "BSD2-Clause license <https://opensource.org/license/bsd-2-clause> (password encryption)\n");
    printf("Type 'help' for a list of commands.\n");

    int exit_cmd = 0;
    while (!exit_cmd) {
        cmd_interface(&exit_cmd);
    }

    return 0;
}

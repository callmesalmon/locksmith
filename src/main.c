#include <stdio.h>
#include <string.h>

#include "colors.h"
#include "commons.h"
#include "password.h"
#include "crypto.h"

#include "cli.h"
#include "cli_msg.h"
#include "auth.h"

// Stallman does not approve.
#if defined(_WIN32) || defined(__CYGWIN__)
#error "Stallman does not approve. Locksmith does not run on windows-like platforms."
#endif

#define LOCKSMITH_TITLE "\n"                                  \
"    __               __                  _ __  __      \n"   \
"   / /   ____  _____/ /___________ ___  (_) /_/ /_     \n"   \
"  / /   / __ \\/ ___/ //_/ ___/ __ `__ \\/ / __/ __ \\ \n"   \
" / /___/ /_/ / /__/ ,< (__  ) / / / / / / /_/ / / /    \n"   \
"/_____/\\____/\\___/_/|_/____/_/ /_/ /_/_/\\__/_/ /_/  \n\n"
#define LOCKSMITH_VERSION "1.3"
#define LOCKSMITH_HELP_MESSAGE                                      \
"locksmith: Enters the locksmith command shell\n"                   \
"Usage: locksmith [-h, --help] [-s --save-backups]\n"              \
"If you ran 'sudo ./install_man.sh' when compiling this program,\n" \
"open the manual with 'man locksmith' for more advanced help.\n"

int main(int argc, char **argv) {
    int save_backups_flag = 0;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
            printf(LOCKSMITH_HELP_MESSAGE);
            return 0;
        }
        if (!strcmp(argv[i], "--save-backups") || !strcmp(argv[i], "-s")) {
            save_backups_flag = 1;
        }
        else {
            cli_warn("'%s' is not a valid flag! Skipping...\n", argv[i]);
        }
    }

    mkdir_if_not_exist(LOCKSMITH_DIR);
    mkdir_if_not_exist(LOCKSMITH_PASSW_DIR);
    mkdir_if_not_exist(LOCKSMITH_BACKUP_DIR);

    create_key(LOCKSMITH_KEY_FILE);

    cli_init();

    if (!save_backups_flag)
        clean_backups();

    auth_master_password();

    printf_color(CYAN, LOCKSMITH_TITLE);

    printf("Locksmith version %s\n", LOCKSMITH_VERSION);
    printf("License: GPLv2-or-later <https://spdx.org/licenses/GPL-2.0-or-later.html> (original code),\n"
           "BSD2-Clause license <https://opensource.org/license/bsd-2-clause> (password encryption)\n");
    printf("Type 'help' for a list of commands.\n");

    while (1) {
        cmd_interface();
    }

    return 0;
}

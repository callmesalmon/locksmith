#include <stdio.h>
#include <string.h>

#include "auth.h"
#include "password.h"
#include "cli_msg.h"
#include "crypto.h"

static MasterPassword mpa;

int master_password_correct(MasterPassword master_pass) {
    unsigned char password_hash[MAX_HASH_LEN];
    hash_password(master_pass.password, password_hash);

    if (!strcmp(master_pass.password, "")) {
        return 0;
    }

    for (int i = 0; master_pass.password[i] != '\0'; i++) {
        if (password_hash[i] != master_pass.hash[i])
            return 0;
    }

    return 1;
}

int create_master_password(char master_password[MAX_STRING_LEN]) {
    unsigned char master_password_hash[MAX_HASH_LEN];

    // Since '0' is the exit string
    if (!strcmp(master_password, "0")) {
        cli_error_die(-1, "'0' is a forbidden password!\n"
            "Exiting...\n");
    }

    hash_password(master_password, master_password_hash);

    mpa.fptr = fopen(LOCKSMITH_MASTER_PASSW_FILE, "wb");
    fwrite(master_password_hash, 1, MAX_HASH_LEN, mpa.fptr);
    fclose(mpa.fptr);

    return 0;
}

int check_master_password() {
    fread(mpa.hash, 1, MAX_HASH_LEN, mpa.fptr);

    int password_verified = 0;
    while (!password_verified) {
        if (mpa.num_tries >= 5) {
            cli_error_die(-1, "Exceeded number of tries allowed for master password!\n"
                "Please try again later.\n");
        }

        printf("Enter master password (0 to exit):\n> ");
        get_string_secret(mpa.password);

        if (!strcmp(mpa.password, "0"))
            exit(0);
        if (master_password_correct(mpa))
            password_verified = 1;
        else {
            cli_error("Wrong password!\n\n");
        }

        mpa.num_tries++;
    }

    fclose(mpa.fptr);

    return 0;
}

int auth_master_password() {
    mpa.fptr = fopen(LOCKSMITH_MASTER_PASSW_FILE, "rb");
    if (mpa.fptr == NULL) {
        char master_password[MAX_STRING_LEN];

        printf("Create master password:\n> ");
        get_string_secret(master_password);

        create_master_password(master_password);
        return 0;
    }
    
    check_master_password();

    return 0;
}

#ifndef AUTH_H
#define AUTH_H

int master_password_correct(char* password, unsigned char* hash);
int create_master_password();
int check_master_password();
int auth_master_password();

#endif

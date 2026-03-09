# locksmith
``locksmith`` is a password manager for Unix-like systems written in C. It aims at being simple and fast with
no unnessecary bloat.

[![Locksmith image](https://github.com/callmesalmon/locksmith/raw/main/screenshot.png)](https://github.com/callmesalmon/locksmith)  
Locksmith in action! _(screenshot may be outdated)_

> [!WARNING]
> THIS SOFTWARE CAN AND WILL NOT GUARANTEE TOTAL SAFETY OF STORED INFORMATION,
> AND ITS CREATORS WILL NOT BE HELD LIABLE FOR ANY SECURITY BREACHES CAUSED BY
> THIS SOFTWARE.

## Features
Locksmith comes with many features, such as:
- Password encryption
- Master password authentication (w/ hashing)
- Atleast a somewhat functional implementation of password backups
- A full "locksmith shell" to handle passwords in.

## Requirements
If you want to build locksmith from source, you need:
- [cc/gcc](https://gcc.gnu.org/)
- [make](https://www.gnu.org/software/make/)
- [libsodium](https://github.com/jedisct1/libsodium)
### Optional requirements
Additionally, if you want the best experience, you need:
- A terminal supporting ANSI color escape codes (locksmith uses these for colored I/O)
- A monospaced font (the title will look really weird otherwise)

## Install
Locksmith will not compile on spyware such as Windows.
```sh
git clone https://github.com/callmesalmon/locksmith
cd locksmith

## NOTE: You may alternatively run
## ``sudo make install`` to install binary
## to /usr/local/bin
make
sudo ./install_man.sh # OPTIONAL: this builds the locksmith manpage
```

## Project layout
The source code of the project is split into different files, with different
purposes. See:

```
src
├── auth.c      Master password authentication
├── auth.h      Master password authentication
├── cli.c       Interface/command shell
├── cli.h       Interface/command shell
├── cli_msg.c   Interface Error/Warning/Info messages
├── cli_msg.h   Interface Error/Warning/Info messages
├── colors.h    Colored I/O
├── commons.c   Common functions, independent of the project
├── commons.h   Common functions, independent of the project
├── crypto.c    Cryptography
├── crypto.h    Cryptography
├── main.c      Entry point
├── password.c  Encrypting/Decryption of passwords
└── password.h  Encrypting/Decryption of passwords
```

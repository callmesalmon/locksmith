# locksmith
``locksmith`` is a password manager written in C. It aims at being simple and fast with
no unnessecary bloat.

[![Locksmith image](https://github.com/callmesalmon/locksmith/raw/main/screenshot.png)](https://github.com/callmesalmon/locksmith)

## Requirements
If you want to build locksmith from source, you need:
- [cc/gcc](https://gcc.gnu.org/)
- [make](https://www.gnu.org/software/make/)
- [libsodium](https://github.com/jedisct1/libsodium)
### Optional requirements
Additionally, if you want the best experience, you need:
- A coloured terminal

## Install
```sh
git clone https://github.com/callmesalmon/locksmith
cd locksmith

## NOTE: You may alternatively run
## ``sudo make install`` to install binary
## to /usr/local/bin
make
```

# locksmith
``locksmith`` is an incredibly unsafe password manager and sometimes password generator written in C.
It is simple and fast; and if your code is that sleek, who cares about safety?

The reason it's so incredibly unsafe is because of my irrational fear of libraries, so I ended up
writing my own 100% certified unsafe encryption/decryption tool. It uses an XOR cipher, which is one
of the most infamously unsafe ciphers.

## Install
```sh
make
```
## Run
```sh
./locksmith
```

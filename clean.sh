#!/usr/bin/sh

config_dir="$HOME/.locksmith"

if [ -d "$config_dir" ]; then
    rm -rf "$config_dir"
    if [ ! $? == 0 ]; then
        exit -1
    fi
fi

man_file="/usr/local/share/man/man1"
if [ -f $man_file ]; then
    rm $man_file
fi

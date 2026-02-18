#!/bin/bash

config_dir="$HOME/.locksmith"

if [ -d "$config_dir" ]; then
    rm -rf "$config_dir"
    if [ ! $? == 0 ]; then
        exit -1
    fi
fi

#!/bin/sh
######## DO NOT MODIFY ########################################################

git submodule update --init
mkdir -p build
cd build
qmake -Wall -spec linux-clang CONFIG+=address_sanitizer ..
make --quiet

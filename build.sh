#!/usr/bin/bash

mkdir tmp

make clean all -E "TARGET := UNIX"
mv bin/* tmp

make clean all -E "TARGET := WIN"
mv bin/* tmp

make clean
mv tmp/* bin
rmdir tmp
#!/usr/bin/bash

make clean all -E "TARGET := UNIX"
mv libezgfx.so bin/

make clean all -E "TARGET := WIN"
mv libezgfx.dll bin/
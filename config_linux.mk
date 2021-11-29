#plateform
CORE := ezGfx_core_linux_xorg

#dependencies
LIBS := -lm -lX11 -lpthread

#the compiler and some flags
CC := gcc
CFLAGS := -O3 -Wall -Iinclude

OBJFLAGS := -c -fPIC
LIBFLAGS := -shared

#output target
LIB := libezgfx.so


define forceremove
    rm -rf $(1)
endef
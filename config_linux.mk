TARGET := test
TARGET_EXEC := $(TARGET)

#plateform
CORE := ezGfx_core_linux_xorg

#ezgfx dependencies
LIBS := -lm
LIBS += -lX11 -lpthread	# linux xorg

#the compiler and some flags
CC := gcc
CFLAGS := -O3 -Wall -Werror=vla -Iinclude

OBJFLAGS := -c -fPIC
LIBFLAGS := -shared -lc
TGTFLAGS := -Wl,-rpath,./ -L./ -lezgfx

MAKE := make
LIB := libezgfx.so

define forceremove
    rm -rf $(1)
endef
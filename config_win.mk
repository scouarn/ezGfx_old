#plateform
CORE := ezGfx_core_win

#dependencies
LIBS := -lm -lGdi32

#the compiler and some flags
CC := gcc 
CFLAGS := -O3 -Wall -Iinclude
OBJFLAGS := -c -DBUILD_DLL
LIBFLAGS := -shared

#output target
LIB := libezgfx.dll

define forceremove
    del /F /Q $(subst /,\\, $(1))
endef

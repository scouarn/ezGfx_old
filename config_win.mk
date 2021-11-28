TARGET := test
TARGET_EXEC := $(TARGET).exe

#plateform
CORE := ezGfx_core_win

#ezgfx dependencies
LIBS := -lm
LIBS += -lGdi32	# windows GDI



#the compiler and some flags
CC := gcc 
CFLAGS := -O3 -Wall -Werror=vla -Iinclude
OBJFLAGS := -c -DBUILD_DLL
LIBFLAGS := -shared
TGTFLAGS := -Wl,-rpath,./ -L./ -lezgfx

MAKE := mingw32-make
LIB := libezgfx.dll

define forceremove
    del /F /Q $(subst /,\\, $(1))
endef

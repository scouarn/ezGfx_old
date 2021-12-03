# HOST := UNIX 
# HOST := WIN

# TARGET := UNIX
# TARGET := WIN

CFLAGS := -O3 -Wall -Iinclude



ifndef HOST
	ifeq ($(OS),Windows_NT)
		HOST := WIN
	else
		HOST := UNIX
	endif 
else 
	#remove white spaces
	HOST := $(strip $(HOST))
endif


ifndef TARGET
	TARGET := $(HOST)
endif



#undefine default CC value
CC =

ifeq ($(TARGET),UNIX)

	CORE := ezGfx_core_linux_xorg
	LIB := bin/libezgfx.so

	LIBS := -lX11 -lpthread
	OBJFLAGS := -c -fPIC
	LIBFLAGS := -shared

	ifeq ($(HOST),UNIX)
		CC := gcc
	endif #no windows-to-linux crosscompiler ?


else ifeq ($(TARGET),WIN)

	CORE := ezGfx_core_win_gdi
	LIB := bin/libezgfx.dll

	LIBS := -lgdi32
	OBJFLAGS := -c -DBUILD_DLL
	LIBFLAGS := -shared

	ifeq ($(HOST),UNIX)
		CC := x86_64-w64-mingw32-gcc #put your crosscompiler here

	else ifeq ($(HOST),WIN)
		CC := mingw32-gcc

	endif

endif


#defining the correct command for cleaning

ifeq ($(HOST),UNIX)
	forceremove = rm -rf $(1)

else ifeq ($(HOST),WIN)
	forceremove = del /F /Q $(subst /,\\, $(1))
endif



#check if most things are correct
ifndef CC
$(error No compiler defined)
endif

ifndef LIB
$(error No target lib file defined)
endif

ifndef CORE
$(error No core implementation defined)
endif
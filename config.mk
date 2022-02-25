
CFLAGS := -O3 -Wall 
INCLUDE := -I../../include
LINK := -L../../bin


ifeq ($(OS),Windows_NT)
	
# 	CC := mingw32-gcc
	CC := x86_64-w64-mingw32-gcc

	LIBS := -lgdi32
	OBJFLAGS := -c -DBUILD_DLL
	LIBFLAGS := -shared

	CORE := ezGfx_core_win_gdi
	LIB_CORE := libezgfx-core.dll
	LIB_COMMON := libezgfx-common.dll
	INSTALL_PATH := .

	RM = del /F /Q $(subst /,\\, $(1))
	
	MAKE := mingw32-make
	CP := cp


else 

	CC := gcc

	LIBS := 
	LIBS += -lX11
	LIBS += -lpthread

	OBJFLAGS := -c -fPIC
	LIBFLAGS := -shared

	CORE := ezGfx_core_linux_xorg
# 	CORE := ezGfx_core_linux_void
	LIB_CORE := libezgfx-core.so
	LIB_COMMON := libezgfx-common.so
	INSTALL_PATH := /usr/local/lib

	RM = rm -rf $(1)
	MAKE := make

endif

CFLAGS := -O3 -Wall -Iinclude


ifeq ($(OS),Windows_NT)
	
	CC := mingw32-gcc

	LIBS := -lgdi32
	OBJFLAGS := -c -DBUILD_DLL
	LIBFLAGS := -shared

	CORE := ezGfx_core_win_gdi
	LIB := bin/libezgfx.dll

	forceremove = del /F /Q $(subst /,\\, $(1))

else 

	CC := gcc

	LIBS := -lX11 -lpthread
	OBJFLAGS := -c -fPIC
	LIBFLAGS := -shared

	CORE := ezGfx_core_linux_xorg
	LIB := bin/libezgfx.so

	forceremove = rm -rf $(1)

endif
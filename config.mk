#choosing plateform
# CORE := ezGfx_core_linux_xorg
CORE := ezGfx_core_win


#ezgfx dependencies
LIBS := -lm 		
# LIBS += -lpthread
# LIBS += -lX11  			#xorg
# LIBS += -lGL  			#openGL
LIBS += -lGdi32				#windows GDI
# LIBS += 					#windows draw2D
# LIBS += 					#windows core audio... 
# LIBS += 					#win32/win64 ?



#the compiler and some flags
CC := gcc 
CFLAGS := -O3 -Wall -Werror=vla -Iinclude

MAKE := make

#files to get backed up
BACKUP := Makefile config.mk TODO.md README.md .git .gitignore sources bin include tools

DYNLIB := libezgfx.so
INSTALL := /usr/lib/

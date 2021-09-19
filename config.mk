#choosing plateform
CORE_VIDEO = ezGfx_core_linux_xorg
CORE_AUDIO = ezGfx_sound_core_linux_alsa


#tool dependencies
LIBS =  -lm -lpthread		#cross plateform base
LIBS += -lasound 			#using alsa
LIBS += -lX11 				#using xorg
# LIBS += -lGL    			#using openGL
# LIBS += 					#windows GDI
# LIBS += 					#windows draw2D
# LIBS += 					#windows core audio... 
# LIBS += 					#win32/win64 ?



#the compiler and some flags
CC = gcc 
CFLAGS = -O3 -Wall -Werror=vla


#files to get backed up
BACKUP = Makefile config.mk TODO.md README.md .git .gitignore sources bin include tools
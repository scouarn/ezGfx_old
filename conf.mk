#project
PROJECT = testing

DEP_PROJECT = $(PROJECT) $(CORE_VIDEO) $(DRAW2D) $(DRAW3D) $(FILES)


#choosing plateform
CORE_VIDEO = ezGfx_core_linux_xorg
CORE_AUDIO = ezGfx_sound_core_linux_alsa

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
CFLAGS = -O3 -Wall
DEBUG_FLAGS = -O0 -Wall -Wextra



#files to be backed up
BACKUP = TODO Makefile README.md assets $(DIR_SRC) $(DIR_BIN) $(DIR_INC)  .git .gitignore 



#directory names
DIR_SRC = sources
DIR_BIN = bin
DIR_BAK = bak
DIR_INC = include



#engine extension files names
DRAW2D = ezGfx_draw2D
DRAW3D = ezGfx_draw3D
FILES  = ezGfx_files

#engine tools name
MUSIC_EDITOR = wavetest
FONT_EDITOR = font_editor
MESH_EDITOR = 3dtest


#tools dependencies
DEP_FONT_EDITOR  = $(FONT_EDITOR)  $(CORE_VIDEO) $(DRAW2D) $(FILES)
DEP_MUSIC_EDITOR = $(MUSIC_EDITOR) $(CORE_VIDEO) $(CORE_AUDIO) $(FILES)
DEP_MESH_EDITOR  = $(MESH_EDITOR)  $(CORE_VIDEO) $(DRAW2D) $(DRAW3D) $(FILES)


#main compile targets
DEP_ALL = $(FONT_EDITOR) $(MUSIC_EDITOR) $(MESH_EDITOR) $(PROJECT)


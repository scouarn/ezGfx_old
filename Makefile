
#used implementation
CORE_VIDEO := ezGfx_core_linux_xorg
CORE_AUDIO := ezSfx_core_linux_alsa


#directories
DIR_SRC := sources
DIR_TGT := sources/entrypoints
DIR_COR := sources/core
DIR_EXT := sources/extensions
DIR_BIN := bin
DIR_INC := include
DIR_REL := release
DIR_BAK := bak

#some list for some operations
RELEASE := TODO Makefile README.md assets $(DIR_SRC) $(DIR_BIN) $(DIR_TGT) $(DIR_INC) 
BACKUP  := $(RELEASE) $(DIR_REL) .git .gitignore 



#the compiler and some flags
CC 	   := gcc 
CFLAGS :=  -O3 -Wall
DEBUG_CFLAG := -Wall
LIBS   := -lm -lpthread -lasound -lX11 #-lGL    #linux


#files handled at some point
HEADERS := $(wildcard $(DIR_INC)/*.h)

SOURCES := $(wildcard $(DIR_EXT)/*.c)
OBJECTS := $(patsubst $(DIR_EXT)/%.c, $(DIR_BIN)/%.o, $(SOURCES)) $(MAINOBJ) $(OBJECTS) $(DIR_BIN)/$(CORE_VIDEO).o $(DIR_BIN)/$(CORE_AUDIO).o

MAINSRC := $(wildcard $(DIR_TGT)/*.c)
MAINOBJ := $(patsubst $(DIR_TGT)/%.c, $(DIR_BIN)/%.o, $(MAINSRC))
TARGETS := $(patsubst $(DIR_TGT)/%.c, $(DIR_BIN)/%,   $(MAINSRC))

COMPILE_ALL := $(OBJECTS) $(MAINOBJ) $(TARGETS)


all : $(COMPILE_ALL)

.PHONY: debug
debug : CFLAGS := $(DEBUG_CFLAG)


#compile objects to executable
$(DIR_BIN)/% : $(DIR_TGT)/%.c $(OBJECTS) $(MAINOBJ)
	$(CC) -I$(DIR_INC) $(CFLAGS) -o $@ $@.o $(OBJECTS) $(LIBS)

#compile sources to objects
$(DIR_BIN)/%.o : $(DIR_SRC)/*/%.c $(HEADERS)
	$(CC) -I$(DIR_INC) $(CFLAGS) -o $@ -c $<




#copy useful stuff in release dir
.PHONY: build
build: $(COMPILE_ALL)
	rm -rf $(DIR_REL)
	mkdir -p $(DIR_REL)

	cp -r $(RELEASE) $(DIR_REL)/

	rm -f $(DIR_REL)/$(DIR_BIN)/*.o
	rm -f $(DIR_REL)/$(DIR_BIN)/*.main

	tar -czf master.tar.gz release/*

#make backup
.PHONY: back
back:
	mkdir -p $(DIR_BAK)
	tar -czf $(DIR_BAK)/backup_$(shell date +'%Y_%d%m_%H%M').tar.gz $(BACKUP)


#remove compiled/built
.PHONY: hardclean
hardclean:
	rm -f $(DIR_BIN)/*
	rm -rf $(DIR_REL)/*
	rm -f master.tar.gz


#remove obj files
.PHONY: clean
clean:
	rm -f $(DIR_BIN)/*


#run test, compile if not up to date
.PHONY : test
test: $(DIR_BIN)/test
	bin/test


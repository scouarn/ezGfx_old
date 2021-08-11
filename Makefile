
#folders
DIR_SRC = sources
DIR_BIN = bin
DIR_TGT = targets
DIR_INC = include
DIR_REL = release
DIR_BAK = bak

RELEASE = TODO Makefile README.md res $(DIR_SRC) $(DIR_BIN) $(DIR_TGT) $(DIR_INC) 
BACKUP  = $(RELEASE) $(DIR_REL) .git .gitignore 


#the compiler and some flags
CC = gcc -I$(DIR_INC) -Wall -O2
LIBS  = -lX11 -lGL -lpthread -lasound -lm #linux


#files handled at some point
MAINSRC := $(wildcard $(DIR_TGT)/*.c)
SOURCES := $(wildcard $(DIR_SRC)/*.c) 
HEADERS := $(wildcard $(DIR_INC)/*.h)

OBJECTS := $(patsubst $(DIR_SRC)/%.c, $(DIR_BIN)/%.o, $(SOURCES))
MAINOBJ := $(patsubst $(DIR_TGT)/%.c, $(DIR_BIN)/%.o, $(MAINSRC))
TARGETS := $(patsubst $(DIR_TGT)/%.c, $(DIR_BIN)/%,   $(MAINSRC))

COMPILE_ALL = $(OBJECTS) $(MAINOBJ) $(TARGETS)

all : $(COMPILE_ALL)

#compile objects to executable
$(DIR_BIN)/% : $(DIR_TGT)/%.c
	$(CC) -o $@ $< $(OBJECTS) $(LIBS)

# Compile sources to objects
$(DIR_BIN)/%.o : $(DIR_SRC)/%.c
	$(CC) -c $< -o $@


$(DIR_BIN)/%.o : $(DIR_TGT)/%.c
	$(CC) -c $< -o $@ 

#recompile if header has changed
# $(SOURCES) : $(HEADERS)
# $(MAINSRC) : $(HEADERS)


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



#folders
DIR_SRC = sources
DIR_BIN = bin
DIR_TGT = targets
DIR_INC = include
DIR_REL = release
DIR_BAK = bak

RELEASE = TODO Makefile README.md $(DIR_SRC) $(DIR_BIN) $(DIR_TGT) $(DIR_INC) 
BACKUP  = $(RELEASE)

#the compiler and some flags
CC = gcc -I$(DIR_INC) #-Wall -O2
LIBS  = -lX11 -lGL -lpthread -lasound -lm #linux



MAINSRC := $(wildcard $(DIR_TGT)/*.c)
MAINOBJ := $(patsubst $(DIR_TGT)/%.c, $(DIR_BIN)/%.main, $(MAINSRC))
TARGETS := $(patsubst $(DIR_TGT)/%.c, $(DIR_BIN)/%,  $(MAINSRC))

SOURCES := $(wildcard $(DIR_SRC)/*.c)
OBJECTS := $(patsubst $(DIR_SRC)/%.c, $(DIR_BIN)/%.o, $(SOURCES))



all: $(OBJECTS) $(TARGETS)


#copy useful stuff in release dir
build: $(OBJECTS) $(TARGETS)
	rm -rf $(DIR_REL)
	mkdir -p $(DIR_REL)

	cp -r $(RELEASE) $(DIR_REL)/

	rm -f $(DIR_REL)/$(DIR_BIN)/*.o
	rm -f $(DIR_REL)/$(DIR_BIN)/*.main

	tar -czf master.tar.gz release/*



bak:
	mkdir -p $(DIR_BAK)
	cp -r $(BACKUP) $(DIR_BAK)/


#remove temp files
clean:
	rm -f bin/*


#link objects to make executables
$(TARGETS) : $(DIR_BIN)/% : $(DIR_BIN)/%.main
	$(CC) -o $@ $< $(OBJECTS) $(LIBS)

#compile main files to object files
$(MAINOBJ) : $(DIR_BIN)/%.main : $(DIR_TGT)/%.c
	$(CC) -c -o $@ $<

# Compile sources to object files
$(OBJECTS) : $(DIR_BIN)/%.o : $(DIR_SRC)/%.c
	$(CC) -c -o $@ $< 
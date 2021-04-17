
#the compiler and some flags
CC = gcc #-Wall -O2
LIBS  = -lX11 -lGL -lpthread -lasound -lm #linux

#folders
DIR_SRC = sources
DIR_BIN = bin
DIR_INC = include

SOURCES := $(wildcard $(DIR_SRC)/*.c)
OBJECTS := $(patsubst $(DIR_SRC)/%.c, $(DIR_BIN)/%.o, $(SOURCES))


#link main file with ezGfx and make the demo exectuable
demo: $(DIR_BIN)/main.o $(OBJECTS)
	$(CC) $(DIR_BIN)/main.o $(OBJECTS) -o demo $(LIBS)

#compile main file to object
$(DIR_BIN)/main.o: main.c
	$(CC) -I$(DIR_INC) -c main.c  -o $(DIR_BIN)/main.o

# Compile to object files
$(OBJECTS): $(DIR_BIN)/%.o : $(DIR_SRC)/%.c
	$(CC) -I$(DIR_INC) -c $< -o $@

#run the demo
run: demo
	./demo


build: $(SOURCES)
	cp -r sources release/
	cp -r include release/
	cp -r bin     release/
	cp   Makefile release/
	cp  README.md release/
	cp     main.c release/
	cp       TODO release/





#remove temp files
clean:
	rm -f bin/*.o release/*

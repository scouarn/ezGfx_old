
#the compiler
CC = gcc

#folders
DIR_SRC = sources
DIR_BIN = bin
DIR_INC = include

LIBS  = -lX11 -lGL -lpthread #linux
SOURCES := $(wildcard $(DIR_SRC)/*.c)
OBJECTS := $(patsubst $(DIR_SRC)/%.c, $(DIR_BIN)/%.o, $(SOURCES))


#link main file with ezGfx and make the demo exectuable
demo: $(DIR_BIN)/main.o $(DIR_BIN)/ezGfx.a
	$(CC) $(DIR_BIN)/main.o $(DIR_BIN)/ezGfx.a -o demo $(LIBS)

#compile main file to object
$(DIR_BIN)/main.o: main.c
	$(CC) -I$(DIR_INC) -c main.c  -o $(DIR_BIN)/main.o

#compile ezGfx to a library file
$(DIR_BIN)/ezGfx.a: $(OBJECTS)
	ar cr $(DIR_BIN)/ezGfx.a $(OBJECTS)

#Compile ezGfx to object files
$(OBJECTS): $(DIR_BIN)/%.o : $(DIR_SRC)/%.c
	$(CC) -I$(DIR_INC) -c $< -o $@


#run the demo
run: demo
	./demo

#remove temp files
clean:
	rm bin/*.o

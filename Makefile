include config.mk


SRC := $(wildcard source/common/*.c) source/core/$(CORE).c
OBJ := $(patsubst %.c,%.o,$(SRC))


DEMOSRC := $(wildcard source/demo/*.c)
DEMO := $(patsubst source/demo/%.c,bin/%,$(DEMOSRC))

CLEAN := $(OBJ) $(wildcard bin/*)

.PHONY : all lib demo

all : lib
all : demo

lib : bin $(LIB)
demo : bin $(LIB) $(DEMO)


clean :
	$(call forceremove,$(CLEAN))


#make so	
$(LIB) : $(OBJ)
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $@ $^ $(LIBS)


#compile demos
bin/% : source/demo/%.c
	$(CC) $(CFLAGS) -o $@ $^ -lm -Wl,-rpath,`pwd`/bin -Lbin -lezgfx

#gitignore wants to ignore the directory itself...
bin :
	mkdir bin

#make objects
%.o : %.c
	$(CC) $(CFLAGS) $(OBJFLAGS) -o $@ $<

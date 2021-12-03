include config.mk


SRC := $(wildcard source/common/*.c) source/core/$(CORE).c
OBJ := $(patsubst %.c,%.o,$(SRC))


DEMOSRC := $(wildcard source/demo/*.c)
DEMO := $(patsubst source/demo/%.c,bin/%,$(DEMOSRC))

CLEAN := $(OBJ) $(wildcard bin/*)

.PHONY : all lib demo

all : lib
all : demo

lib : $(LIB)
demo : $(LIB) $(DEMO)


clean :
	$(call forceremove,$(CLEAN))


#make so	
$(LIB) : $(OBJ)
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $@ $^ $(LIBS)


#compile demos
bin/% : source/demo/%.c
	$(CC) $(CFLAGS) -o $@ $^ -lm -Wl,-rpath,. -Lbin -lezgfx


#make objects
%.o : %.c
	$(CC) $(CFLAGS) $(OBJFLAGS) -o $@ $<

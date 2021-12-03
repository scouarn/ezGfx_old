include config.mk


SRC := $(wildcard source/common/*.c) source/core/$(CORE).c
OBJ := $(patsubst %.c,%.o,$(SRC))


DEMOSRC := $(wildcard demo/*.c)
DEMO := $(patsubst %.c,%,$(DEMOSRC))

CLEAN := $(OBJ) $(LIB) $(DEMO)

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
demo/% : demo/%.c
	$(CC) $(CFLAGS) -o $@ $^ -lm -Wl,-rpath,`pwd` -L. -lezgfx


#make objects
%.o : %.c
	$(CC) $(CFLAGS) $(OBJFLAGS) -o $@ $<

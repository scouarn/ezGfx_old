include config.mk


SRC := $(wildcard source/common/*.c) source/core/$(CORE).c
OBJ := $(patsubst %.c,%.o,$(SRC))

CLEAN := $(OBJ) $(LIB)


all : $(LIB)


clean :
	$(call forceremove,$(CLEAN))


#make so	
$(LIB) : $(OBJ)
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $@ $^ $(LIBS)


#make objects
%.o : %.c
	$(CC) $(CFLAGS) $(OBJFLAGS) -o $@ $<

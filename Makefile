ifeq ($(OS),Windows_NT) 
	include config_win.mk
else
	include config_linux.mk
endif


SRC := $(wildcard source/ext/*.c) source/core/$(CORE).c
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

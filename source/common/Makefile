include ../../config.mk

SRC := $(wildcard *.c)
OBJ := $(patsubst %.c,%.o,$(SRC))

all : $(LIB_COMMON)
	cp $(LIB_COMMON) ../../bin/

clean :
	$(RM) $(CORE).o $(LIB_COMMON)


%.o : %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJFLAGS) -o $@ $<


$(LIB_COMMON) : $(OBJ)
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $@ $^ $(LIBS)
include ../../config.mk

all : $(LIB_CORE)
	cp $(LIB_CORE) ../../bin/


clean :
	$(RM) $(CORE).o $(LIB_CORE)


$(CORE).o : $(CORE).c
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJFLAGS) -o $@ $<


$(LIB_CORE) : $(CORE).o
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $@ $^ $(LIBS)
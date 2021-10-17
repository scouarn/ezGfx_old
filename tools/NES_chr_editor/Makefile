#generic makefile

include ../../config.mk

INCLUDE = ../../include
LINK = $(wildcard ../../bin/*.o)


SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c,%.o,$(SOURCES))

all : main

# dyn lib
# main : $(OBJECTS)
# 	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ $^ $(LIBS) -L../../bin -lezgfx

main : $(OBJECTS)
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ $^ $(LINK) $(LIBS)



%.o : %.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ -c $<

clean :
	rm -f ./*.o main
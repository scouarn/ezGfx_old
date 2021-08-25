#generic makefile

include ../../config.mk

INCLUDE = ../../include
LINK = $(wildcard ../../bin/*.o)

SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c,%.o,$(SOURCES))

all : main

main : $(OBJECTS)
	$(CC) $(CFLAGS) -I$(INCLUDE) $(LINK) $(LIBS) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ -c $^

clean :
	rm -f ./*.o main
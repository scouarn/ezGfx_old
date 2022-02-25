include ../../config.mk


SRC := $(wildcard *.c)
EXE := $(patsubst %.c,%,$(SRC))

all : $(EXE)
	cp $(EXE) ../../bin

clean :
	$(RM) $(EXE)

% : %.c 
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $^ $(LINK) -lm -lezgfx-core -lezgfx-common
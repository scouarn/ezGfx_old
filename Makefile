ifeq ($(OS),Windows_NT) 
	include config_win.mk
else
	include config_linux.mk
endif


EXT_SRC := $(wildcard sources/ext/*.c)
EXT_OBJ := $(patsubst %.c,%.o,$(EXT_SRC))

CORE_SRC := sources/core/$(CORE).c
CORE_OBJ := sources/core/$(CORE).o

CLEAN := $(DYNLIB) $(EXT_OBJ) $(CORE_OBJ) $(LIB) $(TARGET_EXEC) $(TARGET).o

.PHONY: all lib clean

all: $(TARGET_EXEC)
	@echo ALL: SUCCESS


$(TARGET_EXEC) : $(TARGET).o $(LIB)
	$(CC) $(CFLAGS) $(TGTFLAGS) -o $@ $^ 
	@echo TEST: SUCCESS


#make only the so
lib : $(LIB)
	@echo LIB: SUCCESS


#make shared object	
$(LIB) : $(CORE_OBJ) $(EXT_OBJ)
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $@ $^ $(LIBS)


#make objects
$(TARGET).o : $(TARGET).c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o : %.c
	$(CC) $(CFLAGS) $(OBJFLAGS) -o $@ $<


#remove obj files and executables
clean:
	$(call forceremove,$(CLEAN))

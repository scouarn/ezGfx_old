include config.mk


EXT_SRC := $(wildcard sources/ext/*.c)
EXT_OBJ := $(patsubst %.c,%.o,$(EXT_SRC))

CORE_SRC := sources/core/$(CORE).c
CORE_OBJ := sources/core/$(CORE).o


.PHONY: all lib clean install back

all: test

test: test.o $(CORE_OBJ) $(EXT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^
# 	$(CC) $(CFLAGS) -Wl,-rpath,./ -L./ -lezgfx -o $@ $^


#make only the so
dlib : $(DYNLIB)
	@echo DYNLIB: SUCCESS

#copy to global lib directory
install : $(DYNLIB)
	cp $(DYNLIB) $(INSTALL)


#make shared object	
$(DYNLIB) : $(CORE_OBJ) $(EXT_OBJ)
	$(CC) $(CFLAGS) $(LIBS) -shared -lc -o $@ $^

#make objects
%.o : %.c
	$(CC) $(CFLAGS) -fPIC -Iinclude -o $@ -c $<


#remove obj files and executables
clean: $(TOOLS)
	rm -f *.o sources/core/*.o sources/ext/*.o test


#make backup
back:
	mkdir -p .bak
	tar -czf .bak/backup_$(shell date +'%Y_%d%m_%H%M').tar.gz $(BACKUP)




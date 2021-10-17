include config.mk


EXT_SOURCES := $(wildcard sources/ext/*.c)
EXT_OBJECTS := $(patsubst sources/ext/%.c,bin/%.o,$(EXT_SOURCES))

CORE_SOURCES := sources/core/$(CORE_VIDEO).c sources/core/$(CORE_AUDIO).c
CORE_OBJECTS := bin/$(CORE_VIDEO).o bin/$(CORE_AUDIO).o

TOOLS := $(wildcard tools/*)


.PHONY: all dlib slib tools clean install back $(TOOLS)


#make everything
all : dlib
all : slib
all : tools
	@echo ALL: SUCCESS

#make only the objects
slib : $(EXT_OBJECTS) $(CORE_OBJECTS)
	@echo SLIB: SUCCESS

#make only the so
dlib : $(DYNLIB)
	@echo DYNLIB: SUCCESS

#make the tools
tools : $(TOOLS)
	@echo making $(TOOLS)
	@echo TOOLS: SUCCESS

#copy to global lib directory
install : $(DYNLIB)
	cp $(DYNLIB) $(INSTALL)


#make shared object	
$(DYNLIB) : $(CORE_OBJECTS) $(EXT_OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) -shared -lc -Iinclude -o $@ $^

#make objects
bin/%.o : sources/*/%.c
	$(CC) $(CFLAGS) -fPIC -Iinclude -o $@ -c $<

#recursive call inside the tool subdirs
#all / clean targets are used
$(TOOLS) :
	@echo $@
	$(MAKE) -C $@


#remove obj files and executables
#call make clean recursively
clean: $(TOOLS)
	rm -f bin/*


#make backup
back:
	mkdir -p .bak
	tar -czf .bak/backup_$(shell date +'%Y_%d%m_%H%M').tar.gz $(BACKUP)




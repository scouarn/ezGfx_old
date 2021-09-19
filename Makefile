include config.mk


EXT_SOURCES := $(wildcard sources/ext/*.c)
EXT_OBJECTS := $(patsubst sources/ext/%.c,bin/%.o,$(EXT_SOURCES))

CORE_SOURCES := sources/core/$(CORE_VIDEO).c sources/core/$(CORE_AUDIO).c
CORE_OBJECTS := bin/$(CORE_VIDEO).o bin/$(CORE_AUDIO).o

TOOLS := $(wildcard tools/*)

DYNLIB = bin/libezgfx.so


#make everything
.PHONY: all
all : $(TOOLS) $(DYNLIB)
	@echo ALL: SUCCESS

#make only the objects
.PHONY: slib
slib : $(EXT_OBJECTS) $(CORE_OBJECTS)
	@echo SLIB: SUCCESS

#make only the so
.PHONY: dlib
dlib : $(DYNLIB)
	@echo DYNLIB: SUCCESS

#make the tools
.PHONY: tools $(TOOLS)
tools : $(TOOLS)
	@echo TOOLS: SUCCESS


#make shared object	
$(DYNLIB) : $(EXT_SOURCES) $(CORE_SOURCES)
	$(CC) $(CFLAGS) -Iinclude -fPIC -shared -o $@ $^

#make objects
bin/%.o : sources/*/%.c
	$(CC) $(CFLAGS) -Iinclude -o $@ -c $<

#recursive call inside the tool subdirs
#all / clean targets are used
$(TOOLS) : $(EXT_OBJECTS) $(CORE_OBJECTS)
	$(MAKE) -C $@ $(MAKECMDGOALS) 


#remove obj files and executables
#call make clean recursively
clean: $(TOOLS)
	rm -f bin/*


#make backup
.PHONY: back
back:
	mkdir -p .bak
	tar -czf .bak/backup_$(shell date +'%Y_%d%m_%H%M').tar.gz $(BACKUP)




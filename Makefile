include config.mk


EXT_SOURCES := $(wildcard sources/ext/*.c)
EXT_OBJECTS := $(patsubst sources/ext/%.c,bin/%.o,$(EXT_SOURCES))

CORE_SOURCES := sources/core/$(CORE_VIDEO).c sources/core/$(CORE_AUDIO).c
CORE_OBJECTS := bin/$(CORE_VIDEO).o bin/$(CORE_AUDIO).o

TOOLS := $(wildcard tools/*)


#make the lib and the tools
.PHONY: all
all : $(EXT_OBJECTS) $(CORE_OBJECTS) $(TOOLS)
	@echo ALL: SUCCESS

#make only the lib
.PHONY: lib
lib : $(EXT_OBJECTS) $(CORE_OBJECTS)
	@echo LIB: SUCCESS


#make objects
bin/%.o : sources/*/%.c
	$(CC) $(CFLAGS) -Iinclude -o $@ -c $<


#make the tools
.PHONY: tools $(TOOLS)
tools : $(TOOLS)
	@echo TOOLS: SUCCESS

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
	mkdir -p $(DIR_BAK)
	tar -czf $(DIR_BAK)/backup_$(shell date +'%Y_%d%m_%H%M').tar.gz $(BACKUP)




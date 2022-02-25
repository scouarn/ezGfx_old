include config.mk


.PHONY : all lib demo

all : $(LIB_CORE) $(LIB_COMMON) demo

install : $(LIB_CORE) $(LIB_COMMON)
	cp bin/$(LIB_CORE) bin/$(LIB_COMMON) $(INSTALL_PATH)

uninstall :
	$(RM) $(INSTALL_PATH)/$(LIB_CORE) $(INSTALL_PATH)/$(LIB_COMMON)


$(LIB_CORE) : bin
	$(MAKE) -C source/core
	
$(LIB_COMMON) : bin
	$(MAKE) -C source/common

demo : bin
	$(MAKE) -C source/demo

bin :
	mkdir bin



clean :
	$(MAKE) -C source/demo clean
	$(MAKE) -C source/core clean
	$(MAKE) -C source/common clean

	$(RM) $(wildcard bin/*)
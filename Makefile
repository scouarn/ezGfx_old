include conf.mk

#functions
objof = $(DIR_BIN)/$(1).o
exeof = $(DIR_BIN)/$(1)
map   = $(foreach a,$(2),$(call $(1),$(a)))




#main target
COMPILE_ALL := $(call map,objof,$(DEP_ALL)) $(call map,exeof,$(DEP_ALL))


all : $(COMPILE_ALL)


.PHONY: debug
debug : CFLAGS := $(DEBUG_FLAGS)
debug : $(COMPILE_ALL)



#compile executables
$(call exeof,$(FONT_EDITOR))   : $(call map,objof,$(DEP_FONT_EDITOR))
	$(CC) -I$(DIR_INC) $(CFLAGS) $(LIBS) -o $@ $^

$(call exeof,$(MUSIC_EDITOR)) : $(call map,objof,$(DEP_MUSIC_EDITOR))
	$(CC) -I$(DIR_INC) $(CFLAGS) $(LIBS) -o $@ $^

$(call exeof,$(MESH_EDITOR))  : $(call map,objof,$(DEP_MESH_EDITOR))
	$(CC) -I$(DIR_INC) $(CFLAGS) $(LIBS) -o $@ $^


$(call exeof,$(PROJECT))  : $(call map,objof,$(DEP_PROJECT))
	$(CC) -I$(DIR_INC) $(CFLAGS) $(LIBS) -o $@ $^



#compile any source to object
$(DIR_BIN)/%.o : $(DIR_SRC)/*/%.c
	$(CC) -I$(DIR_INC) $(CFLAGS) -o $@ -c $<





#make backup
.PHONY: back
back:
	mkdir -p $(DIR_BAK)
	tar -czf $(DIR_BAK)/backup_$(shell date +'%Y_%d%m_%H%M').tar.gz $(BACKUP)


#remove obj files and executables
.PHONY: clean
clean:
	rm -f $(DIR_BIN)/*

#remove obj files
.PHONY: softclean
softclean:
	rm -f $(DIR_BIN)/*.o
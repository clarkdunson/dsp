.PHONY: all vars $(REGRESS)
.PHONY: mkhdrdir mkhdrs cleanhdrs cleanhdrdir
.PHONY: mkobjdir mkobjs cleanobjs cleanobjdir
.PHONY: mklibdir mklibs cleanlibs 
.PHONY: check clean clobber


all: mkhdrs mkobjs mklibs

vars:
	@echo "Install header dir: _________________"
	@echo $(LIB_INCL_DIR)
	@echo "Object build directory"
	@echo $(LIB_OBJ_DIR)
	@echo "Local header files: _________________"
	@echo $(HDR_FILES)
	@echo "Local source files: _________________"
	@echo $(SRC_FILES)
	@echo "Local object files: _________________"
	@echo $(OBJ_FILES)
	@echo "Target header files: ________________"
	@echo $(INCLUDE_FILES)


# Headers
mkhdrdir:
	mkdir -p $(LIB_INCL_DIR)

mkhdrs: mkhdrdir $(INCLUDE_FILES) $(LIB_INCL_HDR)

cleanhdrs:
	rm -f $(INCLUDE_FILES) $(LIB_INCL_HDR)

cleanhdrdir: cleanhdrs
	# Use rmdir to force removal/cleanup of stale files
	rmdir $(LIB_INCL_DIR)

$(LIB_INCL_HDR): $(LIB_HDR)
	cp $< $@


# Objects
mkobjdir:
	@mkdir -p $(LIB_OBJ_DIR)

mkobjs: mkobjdir $(OBJ_FILES)

cleanobjs: 
	rm -f $(OBJ_FILES)

cleanobjdir: cleanobjs
	# Use rmdir to force removal/cleanup of stale files
	rmdir $(LIB_OBJ_DIR)


# Lib creation
mklibdir: 
	@mkdir -p $(ROOT_OUTPUT_DIR)

mklibs: mklibdir $(STATIC_LIB)

$(STATIC_LIB): mkobjdir $(OBJ_FILES)
	ar -rc $@ $(OBJ_FILES)
	ranlib $@

cleanlibs:
	rm -f $(STATIC_LIB)

clean: cleanlibs cleanobjs
	rm -f $(REGRESS)

# Clean for git
clobber: cleanlibs cleanobjdir cleanhdrdir



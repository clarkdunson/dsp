
SUBDIRS := libCore libDSP

.PHONY: all clobber $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	make -C $@

clobber:
	make -C libDSP clobber
	make -C libCore clobber
	rmdir lib/
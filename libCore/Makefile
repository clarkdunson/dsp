# Copyright ShotSpotter, 2016

HDR_FILES = DSPCommon.h \
            TimeObj.h \
            LogObj.h \
            LockObj.h

LIB_NAME := libCore

include ../mak/defs.mak

include ../mak/rules.mak

REGRESS := regress

# Header Publishing
$(LIB_INCL_DIR)/DSPCommon.h: DSPCommon.h
	cp $< $@

$(LIB_INCL_DIR)/TimeObj.h: TimeObj.h DSPCommon.h
	cp $< $@

$(LIB_INCL_DIR)/LogObj.h: LogObj.h DSPCommon.h
	cp $< $@

$(LIB_INCL_DIR)/LockObj.h: LockObj.h DSPCommon.h
	cp $< $@

# Object Building
$(LIB_OBJ_DIR)/DSPCommon.o: DSPCommon.cpp DSPCommon.h 
	g++ $(G++_OPTS) -c -o $@ $<

$(LIB_OBJ_DIR)/TimeObj.o: TimeObj.cpp TimeObj.h DSPCommon.h 
	g++ $(G++_OPTS) -c -o $@ $<
	
$(LIB_OBJ_DIR)/LogObj.o: LogObj.cpp LogObj.h DSPCommon.h 
	g++ $(G++_OPTS) -c -o $@ $<
	
$(LIB_OBJ_DIR)/LockObj.o: LockObj.cpp LockObj.h DSPCommon.h 
	g++ $(G++_OPTS) -c -o $@ $<
	
$(REGRESS): $(REGRESS).cpp $(SRC_FILES) $(INCLUDE_FILES)
	g++ $(G++_OPTS) -I$(ARTEMIS_INCLS) -L$(ARTEMIS_ROOT)/lib -o $@ $< -lCore


.PHONY: check

check:
	env TZ=UTC ./regressCore
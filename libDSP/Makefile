# Copyright ShotSpotter, 2016

HDR_FILES = DataCommon.h \
            TimeData.h \
            FreqData.h \
            SpecData.h \
            EventData.h 

LIB_NAME := libDSP

include ../mak/defs.mak

LIB_CORE_HDRS := DSPCommon.h TimeObj.h LogObj.h LockObj.h
LIB_CORE_INCLUDES := $(addprefix $(ROOT_INCL_DIR)/libCore/, $(LIB_CORE_HDRS) )

include ../mak/rules.mak


# Header Publishing
$(LIB_INCL_DIR)/DataCommon.h: DataCommon.h $(LIB_CORE_INCLUDES)
	cp $< $@

$(LIB_INCL_DIR)/TimeData.h: TimeData.h DataCommon.h
	cp $< $@

$(LIB_INCL_DIR)/FreqData.h: FreqData.h DataCommon.h
	cp $< $@

$(LIB_INCL_DIR)/SpecData.h: SpecData.h DataCommon.h
	cp $< $@

# $(LIB_INCLS)/DiscreteData.h: DiscreteData.h DataCommon.h
# 	cp $< $@

$(LIB_INCL_DIR)/EventData.h: EventData.h DataCommon.h
	cp $< $@


# Objects
$(LIB_OBJ_DIR)/DataCommon.o: DataCommon.cpp DataCommon.h
	${CC} $(G++_OPTS) -I$(ROOT_INCL_DIR) -c -o $@ $<

$(LIB_OBJ_DIR)/TimeData.o: TimeData.cpp TimeData.h DataCommon.h
	${CC} $(G++_OPTS) -I$(ROOT_INCL_DIR) -c -o $@ $<

$(LIB_OBJ_DIR)/FreqData.o: FreqData.cpp FreqData.h DataCommon.h
	${CC} $(G++_OPTS) -I$(ROOT_INCL_DIR) -c -o $@ $<

$(LIB_OBJ_DIR)/SpecData.o: SpecData.cpp SpecData.h DataCommon.h
	${CC} $(G++_OPTS) -I$(ROOT_INCL_DIR) -c -o $@ $<

# DiscreteData.o: DiscreteData.cpp DiscreteData.h DataCommon.h
# 	${CC} $(G++_OPTS) -I$(ROOT_INCL_DIR) -c -o $@ $<

$(LIB_OBJ_DIR)/EventData.o: EventData.cpp EventData.h DataCommon.h
	${CC} $(G++_OPTS) -I$(ROOT_INCL_DIR) -c -o $@ $<

$(REGRESS): $(REGRESS).cpp $(SRC_FILES) $(INCLUDE_FILES)
	${CC} $(G++_OPTS) -I$(LIB_INCLS) -L$(ARTEMIS_ROOT)/lib -o $@ $< -lCore -lDSP -lSVG


.PHONY: check

check:
	env TZ=UTC ./regress

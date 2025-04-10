# Common library definitions

#LIB_DIR := $(PWD)
LIB_DIR := $(shell pwd)

ROOT_DIR := $(dir $(LIB_DIR))
ROOT_INCL_DIR := $(ROOT_DIR)include
LIB_INCL_DIR := $(ROOT_INCL_DIR)/$(LIB_NAME)
LIB_OBJ_DIR := $(ROOT_DIR)$(LIB_NAME)/obj
LIB_HDR := $(LIB_NAME).h
LIB_INCL_HDR := $(LIB_INCL_DIR)/$(LIB_HDR)
ROOT_OUTPUT_DIR := $(ROOT_DIR)lib


INCLUDE_FILES := $(addprefix $(LIB_INCL_DIR)/, $(HDR_FILES) )
SRC_FILES := $(HDR_FILES:%.h=%.cpp)
OBJ_FILES := $(HDR_FILES:%.h=$(LIB_OBJ_DIR)/%.o)

STATIC_LIB := $(ROOT_OUTPUT_DIR)/$(LIB_NAME).a

G++_OPTS := -O2 -Wall -g

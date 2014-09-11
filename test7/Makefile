# RDA-style makefile

SOURCES := test7.cpp boostly.cpp
LDLIBS += -lpthread -lrt

# Bypass rdainfo.py
_HOST := linux

RDA_TOP := ..
include $(RDA_TOP)/build.mk

all: $(RDA_EXEC)


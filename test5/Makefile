# RDA-style makefile

SOURCES := test5.cpp protocol.cpp boostly.cpp
LDLIBS += -lboost_date_time -lboost_thread
#LDLIBS += -lboost_system
LDLIBS += -lpthread -lrt

# Bypass rdainfo.py
_HOST := linux

RDA_TOP := ..
include $(RDA_TOP)/build.mk

all: $(RDA_EXEC)


# RDA-style makefile

SOURCES := test4.cpp
LDLIBS += -lboost_chrono -lboost_thread -lboost_system -lpthread -lrt

# Bypass rdainfo.py
_HOST := linux

RDA_TOP := ..
include $(RDA_TOP)/build.mk

all: $(RDA_EXEC)


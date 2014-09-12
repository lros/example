# RDA-style makefile

UBUNTU_MAJOR := $(firstword $(subst ., ,$(word 2,$(shell lsb_release -r))))

SOURCES := test5.cpp protocol.cpp boostly.cpp
ifeq ($(TARGET),linux)
LDLIBS += -lboost_date_time -lboost_thread
ifeq ($(UBUNTU_MAJOR),14)
LDLIBS += -lboost_system
endif
endif
LDLIBS += -lpthread -lrt

# Bypass rdainfo.py
_HOST := linux

RDA_TOP := ..
include $(RDA_TOP)/build.mk

all: $(RDA_EXEC)


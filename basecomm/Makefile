# RDA-style makefile

UBUNTU_MAJOR := $(firstword $(subst ., ,$(word 2,$(shell lsb_release -r))))

SOURCES := dataLink.cpp odTransport.cpp

TESTS := setMotorType.cpp exercise.cpp

#DEFINES += DEBUG_CRC

LDLIBS += -lboost_chrono -lboost_thread -lpthread -lrt
ifeq ($(UBUNTU_MAJOR),12)
LDLIBS += -lboost_system
endif

# Bypass rdainfo.py
_HOST := linux

RDA_TOP := ..
include $(RDA_TOP)/build.mk

all: $(RDA_LIB_STATIC)

#$(_OBJDIR)/dataLink.o: CPPFLAGS += -DDEBUG_DATA
$(_OBJDIR)/odTransport.o: CPPFLAGS += -DDEBUG_EEPROM


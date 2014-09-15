# RDA-style makefile

SOURCES := test4.cpp

LDLIBS += -lpthread

# Bypass rdainfo.py
_HOST := linux

RDA_TOP := ..
include $(RDA_TOP)/build.mk

all: $(RDA_EXEC)

ifeq ($(TARGET),imx27)

LDLIBS += ../../boost-1.48-prebuild-imx27/lib/libboost_thread-gcc41-mt-1_48.a
LDLIBS += ../../boost-1.48-prebuild-imx27/lib/libboost_date_time-gcc41-mt-1_48.a
LDLIBS += ../../boost-1.48-prebuild-imx27/lib/libboost_system-gcc41-mt-1_48.a

else

LDLIBS += -lboost_date_time -lboost_thread -lboost_system

endif


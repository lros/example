# Make rules to cross-compile for the imx27

VGO_CACHE ?= $(HOME)/vgo-cache
CROSSTOOL := $(VGO_CACHE)/toolchain/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin
CC = $(CROSSTOOL)/arm-none-linux-gnueabi-gcc
CXX = $(CROSSTOOL)/arm-none-linux-gnueabi-g++
LD = $(CROSSTOOL)/arm-none-linux-gnueabi-ld
AR = $(CROSSTOOL)/arm-none-linux-gnueabi-ar

# The following flags look like the right things, taken from the log of
# an imx27 build.
_BOTH_FLAGS += \
    -pipe \
    -fno-omit-frame-pointer \
    -rdynamic \
    -mapcs-frame \
    -funwind-tables \
    -fno-optimize-sibling-calls \
    -mno-sched-prolog \
    -Wall \
    -W \
    -Wno-unused \
    -fPIC \
    -D_REENTRANT \
    -DPOSIX

# Added based on my notes http://wiki.net.local/mediawiki/index.php/Steve_fiddling_with_ltib_setup#Architecture_Tangent
_BOTH_FLAGS += -mcpu=arm926ej-s

CFLAGS += $(_BOTH_FLAGS)
CXXFLAGS += $(_BOTH_FLAGS)

#LDFLAGS += -L$(RDA_TOP)/../boost-1.48-prebuild-imx27/lib
#CPPFLAGS += -I$(RDA_TOP)/../boost-1.48-prebuild-imx27/include

LDFLAGS += -L$(RDA_TOP)/../libs/imx27
CPPFLAGS += -I$(RDA_TOP)/../boost/boost_1_55_0


FX2LIBDIR=../../../
BASENAME = cdc-acm-loopback
SOURCES=loopback.c ../common/cdc.c ../common/main.c
DEPS=../common/cdc.h ../common/cdc-config.h
INCLUDES=../common
A51_SOURCES=../common/dscr.a51
PID=0x1004

include $(FX2LIBDIR)lib/fx2.mk

download: build/$(BASENAME).ihx
	  ../../debugdevice_full_duplex/download.sh $<

FX2LIBDIR=../../
BASENAME = bulkloop
SOURCES=bulkloop.c
PID=0x1004

include $(FX2LIBDIR)lib/fx2.mk
include $(FX2LIBDIR)lib/fx2-cdesc.mk

test: test.cpp
	g++ -o test test.cpp -lusb-1.0

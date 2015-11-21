# common make targets for compiling fx2 firmware with C descriptors

BUILDDIR?=build

DSCR_AREA=-DDSCR_AREA=0x3e00
SDCCFLAGS += --std-c99
SOURCES += descriptors.c

$(BUILDDIR)/descriptors_stringtable.h: $(FX2LIBDIR)/utils/generate_stringtable.py descriptors.strings
	$< --header < descriptors.strings > $@

$(BUILDDIR)/descriptors_stringtable.inc: $(FX2LIBDIR)/utils/generate_stringtable.py descriptors.strings
	$< --cfile < descriptors.strings > $@


$(BUILDDIR)/$(BASENAME).ihx: descriptors.c $(BUILDDIR)/descriptors_stringtable.h $(BUILDDIR)/descriptors_stringtable.inc

# Check the descriptors using GCC for better error messages
check-descriptors: $(DEPS)
	gcc -Wall -Werror -I$(FX2LIBDIR)/include descriptors.c
	rm a.out

clean-descriptors:
	rm -f $(foreach ext, h inc, $(BUILDDIR)/descriptors_stringtable.${ext})

clean: clean-descriptors

$(FX2LIBDIR)/include/linux/.git: $(FX2LIBDIR)/.gitmodules
	git submodule update --recursive --init $(dir $@)
	touch $@ -r $(FX2LIBDIR)/.gitmodules

.PHONY: check-descriptors clean-descriptors

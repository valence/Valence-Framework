ADDITIONAL_CFLAGS = -fPIC
INCLUDE = ../../include
EXTRA_CLEAN = ../simulator_$(LIBNAME).so $(LIBOBJS)

$(LIBNAME): $(OBJS) $(LIBOBJS)
	$(CC) -o simulator_`basename $@`.so $@.o $(OBJS) -shared $(CLIBS)
	@mv -v simulator_`basename $@`.so ../

include ../../../make.inc

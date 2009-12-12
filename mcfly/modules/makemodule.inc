ADDITIONAL_CFLAGS = -fPIC
INCLUDE = ../../include
EXTRA_CLEAN = ../libmcfly_$(LIBNAME).so $(LIBOBJS)

$(LIBNAME): $(OBJS) $(LIBOBJS)
	$(CC) -o libmcfly_`basename $@`.so $@.o $(OBJS) -shared $(CLIBS)
	@mv -v libmcfly_`basename $@`.so ../

include ../../make.inc

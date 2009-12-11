ADDITIONAL_CFLAGS = -fPIC
INCLUDE = ../../include
EXTRA_CLEAN = ../libmcfly_$(LIBNAME).so

$(LIBNAME): $(LIBOBJS)
	$(CC) -o libmcfly_`basename $@`.so $@.o -shared $(CLIBS)
	@mv -v libmcfly_`basename $@`.so ../

include ../../make.inc

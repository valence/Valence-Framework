ADDITIONAL_CFLAGS = -fPIC
INCLUDE = ../../include

$(LIBNAME): $(LIBOBJS)
	$(CC) -o libmcfly_`basename $@`.so $@.o -shared $(CLIBS)
	@mv -v libmcfly_`basename $@`.so ../

include ../../make.inc
